/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_vip.c
 *
 *  \brief VPS VIP 0/1 Secondary and HDCOMP path core file.
 *  This file implements the core layer for VIP 0/1 Secondary and HDCOMP paths.
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
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/vpshal_sc.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/vps/core/vpscore_vip.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Maximum number of handles supported per VIP instance. */
#define VCORE_VIP_MAX_HANDLES           (4u)

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_VIP_MAX_HANDLES * VCORE_VIP_MAX_CHANNELS
 *  objects per instance, only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_VIP_MAX_CHANNEL_OBJECTS   (VCORE_VIP_MAX_CHANNELS                \
                                       * VCORE_VIP_NUM_INST)

/** \brief Default width used for initializing format structure. */
#define VCORE_VIP_DEFAULT_WIDTH         (720u)
/** \brief Default height used for initializing format structure. */
#define VCORE_VIP_DEFAULT_HEIGHT        (480u)

/** \brief Time out to be used in sem pend. */
#define VCORE_VIP_SEM_TIMEOUT           (BIOS_WAIT_FOREVER)

/**
 *  \brief Maximum number of VPDMA channels/descriptors to program.
 *  For YUV422 and RGB, only one channel is needed.
 *  For YUV420, two channels are needed.
 */
#define VCORE_VIP_MAX_VPDMA_CH          (2u)

/* Maxmum streams supported in the VIP Core */
#define VCORE_VIP_MAX_STREAMS           (2u)

/* Temporary Definition for the streamId. Current vcore interface
   does not support passing streamId to the core. This should be
   removed once it is supported. */
#define VCORE_VIP_STREAMID              (0u)

#define VCORE_VIP_CORE_NAME             ("fvid2_vps_vip_core_ti")


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Typedef for VIP instance object. */
typedef struct Vcore_VipInstObj_t Vcore_VipInstObj;

/**
 *  struct Vcore_VipSteamObj
 *  \brief Per Stream information.
 */
typedef struct
{
    UInt32                  streamId;
    /**< Id of the stream */
    Vcore_VipParams         corePrms;
    /**< VIP config parameters. */
    Vcore_DescInfo          descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data and config descriptor memories required by this core. */
    UInt32                  numDesc;
    /**< Number of descriptors required for programming. */
    VpsHal_ScConfig         scHalCfg;
    /**< Scalar HAL configuration. */
    Vps_CscConfig           cscHalCfg;
    /**< CSC HAL configuration. */
    VpsHal_VpdmaChannel     vpdmaCh[VCORE_VIP_MAX_VPDMA_CH];
    /**< Array to store the VPDMA channel number required by core. */
    VpsHal_VpdmaChanDT      vpdmaChDataType[VCORE_VIP_MAX_VPDMA_CH];
    /**< Array to store the VPDMA channel data type required by core. */
    Bool                    isModReq[VCORE_VIP_MAX_MOD];
    /**< Flag to indicate whether a module needs to be programmed or not. */
    UInt32                  bufIndex[VCORE_VIP_MAX_VPDMA_CH];
    /**< Buffer index to program the channel address of the VPDMA. */
    UInt32                  ovlyOffset[VCORE_VIP_MAX_HAL];
    /**< Byte offset to overlay memory for each HAL. */
} Vcore_VipStreamObj;

/**
 *  struct Vcore_VipChObj
 *  \brief Per channel information.
 */
typedef struct
{
    Vcore_States            state;
    /**< Current state. */
    Vcore_VipStreamObj      streamObj[VCORE_VIP_MAX_STREAMS];
    /**< Stream Objects */
} Vcore_VipChObj;

/**
 *  struct Vcore_VipHandleObj
 *  \brief Per handle information.
 */
typedef struct
{
    UInt32                  isUsed;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_VipInstObj       *parent;
    /**< Pointer to the instance object. */
    UInt32                  perChCfg;
    /**< Flag indicating whether format is common for all the channels or
         it is different for each channel of a handle. */
    Vcore_VipChObj          handleContext;
    /**< Used when the handle is opened with perChCfg as false. */
    Vcore_VipChObj         *chObjs[VCORE_VIP_MAX_CHANNELS];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool. */
    UInt32                  numCh;
    /**< Number of channels for this handle. This determines the number of
         valid pointers in channel array. */
    Vcore_VipResObj         resObj;
    /**< VIP resource object */
    Vcore_VipResAllocParams resAllocParams;
    /**< VIP resource allocation parameters */
} Vcore_VipHandleObj;

/**
 *  struct Vcore_VipInstObj_t
 *  \brief Structure containing per instance information.
 */
struct Vcore_VipInstObj_t
{
    UInt32                  instId;
    /**< Instance number/ID. */
    VpsHal_VpsVipInst       vipInst;
    /**< VIP instance number to be passsed to VPS HAL for setting VIP mux. */
    UInt32                  numHandle;
    /**< Number of handles for this instance. This determines the size of
         handle array. */

    VpsHal_Handle           halHandle[VCORE_VIP_MAX_HAL];
    /**< Array to store the HAL handles required by core. */
    UInt32                  halShwOvlySize[VCORE_VIP_MAX_HAL];
    /**< Array to store the shadow overlay size for each module. */
    VpsHal_GetCfgOvlySize   getCfgOvlySize[VCORE_VIP_MAX_HAL];
    /**< Array to store the overlay size function pointer for each HAL. */
    VpsHal_CreateCfgOvly    createCfgOvly[VCORE_VIP_MAX_HAL];
    /**< Array to store the create overlay function pointer for each HAL. */
    UInt32                  openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    Vcore_OpMode            curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or mem-mem operation of the core. */
    Vcore_VipHandleObj     *hObjs;
    /**< Pointer to the handle information. This is a contiguous memory of
         numHandle Vcore_VipHandleObj structure. */
    Vcore_VipInSrc          vipInSrc;
    /**< VIP input source for this instance. This will be passed to
         the VIP resource manager */
    UInt32                  numVipOutSteams;
    /**< Number of VIP output formats */
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Vcore_Handle Vcore_vipOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg);
static Int32 Vcore_vipClose(Vcore_Handle handle);

static Int32 Vcore_vipGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property);

static Int32 Vcore_vipSetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                const Void *params);
static Int32 Vcore_vipGetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                Void *params);
static Int32 Vcore_vipControl(Vcore_Handle handle, UInt32 cmd, Ptr args);

static Int32 Vcore_vipGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo);

static Int32 Vcore_vipProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem);
static Int32 Vcore_vipProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_vipProgramScCoeff(Vcore_Handle handle,
                                     UInt32 chNum,
                                     Vcore_ScCoeffParams *coeffPrms);

static Int32 Vcore_vipUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid);
static Int32 Vcore_vipUpdateRtMem(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  UInt32 chNum,
                                  const Void *rtPrms,
                                  UInt32 frmidx);

static Int32 Vcore_vipSetFsEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_vipGetScFactorConfig(Vcore_Handle handle,
                                        UInt32 chNum,
                                        VpsHal_ScFactorConfig *scFactorConfig,
                                        const Vcore_DescMem *descMem);

static Int32 vcoreVipFigureOutState(const Vcore_VipHandleObj *hObj,
                                    Vcore_VipStreamObj *streamObj);

static Int32 vcoreVipSetScHalCfg(Vcore_VipStreamObj *chObj);

static Int32 vcoreVipCheckParams(const Vcore_VipParams *corePrms);

static UInt32 vcoreVipProgramDesc(const Vcore_VipHandleObj *hObj,
                                  const Vcore_VipStreamObj *streamObj,
                                  const Vcore_DescMem *descMem);

static inline Vcore_VipChObj *vcoreVipGetChObj(Vcore_VipHandleObj *hObj,
                                               UInt32 chNum);

static Vcore_VipHandleObj *vcoreVipAllocHandleObj(UInt32 instId,
                                                  Vcore_OpMode mode);
static Int32 vcoreVipFreeHandleObj(Vcore_VipHandleObj *hObj);

static void vcoreVipSetDefChInfo(Vcore_VipChObj *chObj, UInt32 chNum);

static Int32 vcoreVipAllocChObj(Vcore_VipChObj **chObjs, UInt32 numCh);
static Int32 vcoreVipFreeChObj(Vcore_VipChObj **chObjs, UInt32 numCh);

static Vcore_VipHandleObj *vcoreVipAllocHandleMem(UInt32 numHandle);
static Int32 vcoreVipFreeHandleMem(const Vcore_VipHandleObj *hObjs);
static Vcore_VipChObj *vcoreVipAllocChMem(void);
static Int32 vcoreVipFreeChMem(const Vcore_VipChObj *chObj);
static Void vcoreInitResMngrObj(Vcore_VipHandleObj *hObj);
static Int32 vcoreVipScSetAdvConfig(Vcore_VipHandleObj *hObj,
                                    const Vcore_ScCfgParams *scCfgPrms);
static Int32 vcoreVipScGetAdvConfig(Vcore_VipHandleObj *hObj,
                                    Vcore_ScCfgParams *scCfgPrms);
static Int32 vcoreVipAllocPath(Vcore_VipHandleObj *hObj,
                               Vcore_VipChObj *chObj);
static Int32 vcoreVipFreePath(Vcore_VipHandleObj *hObj,
                               Vcore_VipChObj *chObj);
static Int32 vcoreVipSetConfig(Vcore_VipHandleObj *hObj,
                               UInt32 chNum);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief VIP core function pointer. */
static const Vcore_Ops VipCoreOps =
{
    Vcore_vipGetProperty,           /* Get property */
    Vcore_vipOpen,                  /* Open function */
    Vcore_vipClose,                 /* Close function */
    NULL,                           /* Set format */
    NULL,                           /* Get format */
    NULL,                           /* Set mosaic */
    NULL,                           /* Create layout */
    NULL,                           /* Delete layout */
    Vcore_vipGetDescInfo,           /* Get descriptor info */
    Vcore_vipSetFsEvent,            /* Set frame */
    Vcore_vipSetParams,             /* Set parameters */
    Vcore_vipGetParams,             /* Get parameters */
    Vcore_vipControl,               /* Control */
    Vcore_vipProgramDesc,           /* Program descriptor */
    Vcore_vipProgramReg,            /* Program register */
    Vcore_vipUpdateDesc,            /* Update descriptor */
    NULL,                           /* Update multiple window descriptor */
    NULL,                           /* Get context info */
    NULL,                           /* Update context descriptor */
    Vcore_vipUpdateRtMem,           /* Update runtime params */
    Vcore_vipProgramScCoeff,        /* Program scalar coefficient */
    NULL,                           /* Update DEI mode */
    Vcore_vipGetScFactorConfig      /* Get scaling factor config */
};

/** \brief Properties of this core. */
static Vcore_Property VipProperty;

/** \brief VIP core objects. */
static Vcore_VipInstObj VipInstObjects[VCORE_VIP_NUM_INST];

/* ========================================================================== */
/*  All the below variables could be removed if dynamic allocation is done.   */
/* ========================================================================== */
/**
 *  \brief Memory pool for the handle objects per instance.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_VipHandleObj VipHandleMemPool[VCORE_VIP_NUM_INST]
                                          [VCORE_VIP_MAX_HANDLES];

/**
 *  \brief The flag variable represents whether a given handle object is
 *  allocated or not.
 */
static UInt32 VipHandleMemFlag[VCORE_VIP_NUM_INST];

/**
 *  \brief Memory pool for the channel objects.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_VipChObj VipChMemPool[VCORE_VIP_MAX_CHANNEL_OBJECTS];

/**
 *  \brief The flag variable represents whether a given channel object is
 *  allocated or not.
 */
static UInt32 VipChMemFlag[VCORE_VIP_MAX_CHANNEL_OBJECTS];

/**
 *  \brief Semaphore for protecting allocation and freeing of memory
 *  pool objects.
 */
static Semaphore_Handle VipPoolSem;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_vipInit
 *  \brief VIP core init function.
 *  Initializes VIP core objects, allocates memory etc.
 *  This function should be called before calling any of VIP core API's.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initPrms     Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_vipInit(UInt32 numInst,
                    const Vcore_VipInitParams *initPrms,
                    Ptr arg)
{
    Int32               retVal = VPS_SOK;
    UInt32              instCnt, hCnt, chCnt, halCnt;
    Vcore_VipInstObj   *instObj;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(VipCoreTrace, (numInst <= VCORE_VIP_NUM_INST));
    GT_assert(VipCoreTrace, (NULL != initPrms));

    /* Mark pool flags as free */
    for (instCnt = 0u; instCnt < VCORE_VIP_NUM_INST; instCnt++)
    {
        VipHandleMemFlag[instCnt] = FALSE;
        for (hCnt = 0u; hCnt < VCORE_VIP_MAX_HANDLES; hCnt++)
        {
            VipHandleMemPool[instCnt][hCnt].isUsed = FALSE;
        }
    }
    for (chCnt = 0u; chCnt < VCORE_VIP_MAX_CHANNEL_OBJECTS; chCnt++)
    {
        VipChMemFlag[chCnt] = FALSE;
    }

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    VipPoolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == VipPoolSem)
    {
        GT_0trace(VipCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        for (instCnt = 0u; instCnt < numInst; instCnt++)
        {
            /* Initialize instance object members */
            instObj = &VipInstObjects[instCnt];
            instObj->instId = initPrms[instCnt].instId;
            instObj->vipInst = initPrms[instCnt].vipInst;
            instObj->vipInSrc = initPrms[instCnt].vipInSrc;
            instObj->numVipOutSteams = initPrms[instCnt].numVipOutSteams;

            /* Assign get overlay size function pointer */
            instObj->getCfgOvlySize[VCORE_VIP_SC_IDX] =
                VpsHal_scGetConfigOvlySize;
            instObj->getCfgOvlySize[VCORE_VIP_CSC_IDX] =
                VpsHal_cscGetConfigOvlySize;

            /* Assign create overlay function pointer */
            instObj->createCfgOvly[VCORE_VIP_SC_IDX] =
                VpsHal_scCreateConfigOvly;
            instObj->createCfgOvly[VCORE_VIP_CSC_IDX] =
                VpsHal_cscCreateConfigOvly;

            /* Initialize HAL variables */
            for (halCnt = 0u; halCnt < VCORE_VIP_MAX_HAL; halCnt++)
            {
                /* Get the HAL handle */
                instObj->halHandle[halCnt] =
                    initPrms[instCnt].halHandle[halCnt];

                /* Get the overlay size for each of the modules */
                if (NULL != instObj->halHandle[halCnt])
                {
                    instObj->halShwOvlySize[halCnt] =
                        instObj->getCfgOvlySize[halCnt](
                            instObj->halHandle[halCnt]);
                }
                else
                {
                    instObj->halShwOvlySize[halCnt] = 0u;
                }
            }

            instObj->openCnt = 0u;
            instObj->curMode = VCORE_OPMODE_INVALID;

            /* Allocate handle object memory */
            instObj->hObjs = vcoreVipAllocHandleMem(
                                 initPrms[instCnt].maxHandle);
            if (NULL == instObj->hObjs)
            {
                GT_1trace(VipCoreTrace, GT_ERR,
                    "Handle allocation failed for %d!!\n", instCnt);
                retVal = VPS_EALLOC;
                break;
            }
            instObj->numHandle = initPrms[instCnt].maxHandle;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Initialize VIP core properties */
        VipProperty.numInstance = numInst;
        VipProperty.name= VCORE_TYPE_VIP;
        VipProperty.type = VCORE_TYPE_OUTPUT;
        VipProperty.internalContext = FALSE;
    }
    else
    {
        /* Cleanup if error occurs */
        GT_0trace(VipCoreTrace, GT_ERR, "VIP Core Init Failed!!\n");
        Vcore_vipDeInit(NULL);
    }

    return (retVal);
}



/**
 *  Vcore_vipDeInit
 *  \brief VIP core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_vipDeInit(Ptr arg)
{
    Int32               retVal = VPS_SOK;
    UInt32              instCnt, halCnt;
    Vcore_VipInstObj   *instObj;

    /* Free-up handle objects for each instance if already allocated */
    for (instCnt = 0u; instCnt < VCORE_VIP_NUM_INST; instCnt++)
    {
        instObj = &VipInstObjects[instCnt];
        instObj->instId = VCORE_VIP_INST_SEC0;
        instObj->curMode = VCORE_OPMODE_INVALID;
        for (halCnt = 0u; halCnt < VCORE_VIP_MAX_HAL; halCnt++)
        {
            instObj->halHandle[halCnt] = NULL;
            instObj->halShwOvlySize[halCnt] = 0u;
        }
        if (instObj->hObjs)
        {
            vcoreVipFreeHandleMem(instObj->hObjs);
            instObj->hObjs = NULL;
            instObj->numHandle = 0u;
        }
    }

    /* Delete the Semaphore created for pool objects */
    if (VipPoolSem)
    {
        Semaphore_delete(&VipPoolSem);
        VipPoolSem = NULL;
    }

    VipProperty.numInstance = 0u;

    return (retVal);
}



/**
 *  Vcore_vipGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_vipGetCoreOps(void)
{
    return &VipCoreOps;
}



/**
 *  Vcore_vipOpen
 *  \brief Opens a particular instance of VIP core in either capture
 *  or in mem-mem mode and returns the handle to the handle object.
 *  When opened in capture mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in mem-mem mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_VIP_MAX_HANDLES macro. Also each of the handles could have multiple
 *  channel to support more than one mem-mem operation is a single request.
 *
 *  \param instId       Instance to open - VIP 0/1 Secondary or HDCOMP instance.
 *  \param mode         Mode to open the instance for - capture or mem-mem.
 *  \param numCh        Number of channel to be associated with this open.
 *                      For capture operation only one channel is allowed.
 *                      For mem-mem operation, more than one channel could
 *                      be associated with a handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_VIP_MAX_CHANNELS macro.
 *  \param perChCfg     Flag indicating whether the config descriptors
 *                      should be separate for each and every channel or
 *                      not. This is valid only for mem-mem drivers. For
 *                      capture drivers this should be set to FALSE.
 *                      TRUE - Separate config descriptors for each channel.
 *                      FALSE - One config descriptors used per handle.
 *
 *  \return             If success returns the handle else returns NULL.
 */
static Vcore_Handle Vcore_vipOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg)
{
    Vcore_VipHandleObj *hObj = NULL;
    Int32               retVal = VPS_SOK;

    /* Only capture and memory modes are supported */
    GT_assert(VipCoreTrace,
        ((VCORE_OPMODE_CAPTURE == mode) || (VCORE_OPMODE_MEMORY == mode)));
    /* Check for maximum channel supported per handle */
    GT_assert(VipCoreTrace, (numCh <= VCORE_VIP_MAX_CHANNELS));
    /* There should be atleast one channel */
    GT_assert(VipCoreTrace, (0u != numCh));
    /* For capture operation, only one channel should be allowed */
    GT_assert(VipCoreTrace,
        (!((VCORE_OPMODE_CAPTURE == mode) && (VCORE_MAX_CAPTURE_CH < numCh))));
    /* For capture operation, per channel format should be FALSE */
    GT_assert(VipCoreTrace,
        (!((VCORE_OPMODE_CAPTURE == mode) && (FALSE != perChCfg))));

    /* Allocate a handle object */
    hObj = vcoreVipAllocHandleObj(instId, mode);
    if (NULL != hObj)
    {
        /* Allocate channel objects */
        retVal = vcoreVipAllocChObj(hObj->chObjs, numCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(VipCoreTrace, GT_ERR, "Channel allocation failed!!\n");
            /* Free-up handle object if channel allocation failed */
            vcoreVipFreeHandleObj(hObj);
            hObj = NULL;
        }
        else
        {
            /* Initialize variables */
            hObj->perChCfg = perChCfg;
            hObj->numCh = numCh;
            /* Initialize VIP Resource Manager Object */
            vcoreInitResMngrObj(hObj);
        }
    }
    else
    {
        GT_0trace(VipCoreTrace, GT_ERR, "Handle allocation failed!!\n");
    }

    return ((Vcore_Handle) hObj);
}



/**
 *  Vcore_vipClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       VIP core handle.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipClose(Vcore_Handle handle)
{
    Int32                   retVal = VPS_EFAIL;
    Vcore_VipHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Free channel objects */
    retVal = vcoreVipFreeChObj(hObj->chObjs, hObj->numCh);
    /* Free handle objects */
    retVal |= vcoreVipFreeHandleObj(hObj);

    if (VPS_SOK != retVal)
    {
        GT_0trace(VipCoreTrace, GT_ERR, "Free memory failed!!\n");
    }

    return (retVal);
}



/**
 *  Vcore_vipGetProperty
 *  \brief Gets the core properties of the VIP core.
 *
 *  \param handle       VIP core handle.
 *  \param property     Pointer to which the VIP core properties to
 *                      be copied.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property)
{
    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != property));

    VpsUtils_memcpy(property, &VipProperty, sizeof(Vcore_Property));

    return (VPS_SOK);
}



/**
 *  Vcore_vipSetParams
 *  \brief Sets the VIP core parameters for a given channel.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel to which the params should be applied.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param params       Pointer to the params information. This should point to
 *                      a valid Vcore_VipParams structure.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipSetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                const Void *params)
{
    Int32                   retVal = VPS_SOK;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;
    const Vcore_VipParams  *corePrms;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != params));
    hObj = (Vcore_VipHandleObj *) handle;
    corePrms = (Vcore_VipParams *) params;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the parameters are valid */
        retVal = vcoreVipCheckParams(corePrms);
        if (VPS_SOK != retVal)
        {
            GT_0trace(VipCoreTrace, GT_ERR, "Invalid Format!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        /* Copy the params to the local structure */
        VpsUtils_memcpy(
            &(chObj->streamObj[VCORE_VIP_STREAMID].corePrms),
            corePrms,
            sizeof(Vcore_VipParams));
    }

    return (retVal);
}



/**
 *  Vcore_vipGetParams
 *  \brief Gets the VIP core parameters for a given channel.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel from which the params should be read.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param params       Pointer to the copied params information. This should
 *                      point to a valid Vcore_VipParams structure.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipGetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                Void *params)
{
    Int32                   retVal = VPS_SOK;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != params));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        /* Copy the params */
        VpsUtils_memcpy(
            params,
            &chObj->streamObj[VCORE_VIP_STREAMID].corePrms,
            sizeof(Vcore_VipParams));
    }

    return (retVal);
}



/**
 *  Vcore_vipGetDescInfo
 *  \brief
 */
static Int32 Vcore_vipControl(Vcore_Handle handle, UInt32 cmd, Ptr args)
{
    Int32                   retVal = VPS_SOK;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));

    hObj = (Vcore_VipHandleObj *) handle;

    switch (cmd)
    {
        case VCORE_IOCTL_SET_SC_CFG:
            GT_assert(VipCoreTrace, (NULL != args));
            retVal = vcoreVipScSetAdvConfig(hObj, (Vcore_ScCfgParams *) args);
            break;

        case VCORE_IOCTL_GET_SC_CFG:
            GT_assert(VipCoreTrace, (NULL != args));
            retVal = vcoreVipScGetAdvConfig(hObj, (Vcore_ScCfgParams *) args);
            break;

        case VCORE_IOCTL_VIP_ALLOC_PATH:
            /* Caution: Assuming here that if scalar is required
               in any of the channel, isScReq flag is set in
               all the channels */
            chObj = vcoreVipGetChObj(hObj, 0u);
            /* NULL pointer check */
            GT_assert(VipCoreTrace, (NULL != chObj));
            retVal = vcoreVipAllocPath(hObj, chObj);
            break;

        case VCORE_IOCTL_VIP_FREE_PATH:
            /* Caution: Assuming here that if scalar is required
               in any of the channel, isScReq flag is set in
               all the channels */
            chObj = vcoreVipGetChObj(hObj, 0u);
            /* NULL pointer check */
            GT_assert(VipCoreTrace, (NULL != chObj));
            retVal = vcoreVipFreePath(hObj, chObj);
            break;

        /* TODO: Provide proper name to this ioctl */
        case VCORE_IOCTL_VIP_SET_CONFIG:
            GT_assert(VipCoreTrace, (NULL != args));
            retVal = vcoreVipSetConfig(hObj, *(UInt32 *)args);
            break;

        default:
            retVal = VPS_EUNSUPPORTED_CMD;
            break;
    }

    return (retVal);
}



/**
 *  Vcore_vipGetDescInfo
 *  \brief Depending on the params set, returns the number of data
 *  descriptors and config overlay memory needed for the actual driver to
 *  allocate memory.
 *  Params should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel to which the info is required.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param layoutId     [IN] Layout ID for which to return the information.
 *  \param descInfo     Pointer to the number of data descriptor and config
 *                      overlay memory size information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo)
{
    Int32                   retVal = VPS_SOK;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != descInfo));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE == chObj->state.isParamsSet)
        {
            retVal = VPS_SOK;
            /* Copy the data descriptor information */
            VpsUtils_memcpy(
                (Void *) descInfo,
                &(chObj->streamObj[VCORE_VIP_STREAMID].descInfo),
                sizeof(Vcore_DescInfo));
        }
        else
        {
            retVal = VPS_EFAIL;
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before getting the descriptor info!!\n");
        }
    }

    return (retVal);
}



/**
 *  Vcore_vipProgramDesc
 *  \brief Programs the data descriptor for a given channel.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in capture mode, this should be always set
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
static Int32 Vcore_vipProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != descMem));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before programming the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        retVal = vcoreVipProgramDesc(
                    hObj,
                    &(chObj->streamObj[VCORE_VIP_STREAMID]),
                    descMem);
    }

    return (retVal);
}



/**
 *  Vcore_vipProgramReg
 *  \brief Programs the registers for each of the HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param descMem      Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actual registers. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  halCnt;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;
    Vcore_VipInstObj       *instObj;
    Vcore_VipStreamObj     *streamObj;
    Void                   *cfgOvlyPtr = NULL;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    hObj = (Vcore_VipHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before programming the overlay memory!!\n");
            retVal = VPS_EFAIL;
        }
        /* Get the Stream Object from the channel object */
        streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);
    }

    if ((VPS_SOK == retVal) && (NULL != descMem))
    {
        /* Check if overlay pointers are non-NULL for the required overlay */
        GT_assert(VipCoreTrace,
            !((0u != streamObj->descInfo.shadowOvlySize)
            && (NULL == descMem->shadowOvlyMem)));
        GT_assert(VipCoreTrace,
            !((0u != streamObj->descInfo.nonShadowOvlySize)
            && (NULL == descMem->nonShadowOvlyMem)));

        /* Create overlay memory for each of the HAL */
        for (halCnt = 0u; halCnt < VCORE_VIP_MAX_HAL; halCnt++)
        {
            if (TRUE == streamObj->isModReq[halCnt])
            {
                /* NULL pointer check */
                GT_assert(VipCoreTrace,
                    (NULL != instObj->createCfgOvly[halCnt]));
                GT_assert(VipCoreTrace, (instObj->halHandle[halCnt]));

                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                                      + streamObj->ovlyOffset[halCnt]);
                retVal = instObj->createCfgOvly[halCnt](
                             instObj->halHandle[halCnt],
                             cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_1trace(VipCoreTrace, GT_ERR,
                        "HAL Create Overlay Failed for HAL Id: %d!!\n", halCnt);
                    break;
                }
            }
        }
    }

    /* Configure Scalar Module */
    if (VPS_SOK == retVal)
    {
        if (TRUE == streamObj->isModReq[VCORE_VIP_SC_IDX])
        {
            if (NULL != descMem)
            {
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + streamObj->ovlyOffset[VCORE_VIP_SC_IDX]);
            }
            retVal = VpsHal_scSetConfig(
                         instObj->halHandle[VCORE_VIP_SC_IDX],
                         &streamObj->scHalCfg,
                         cfgOvlyPtr);
            if (VPS_SOK != retVal)
            {
                GT_0trace(VipCoreTrace, GT_ERR,
                    "SCALAR HAL Create Overlay Failed!!\n");
            }
        }
    }

    /* Configure CSC Module */
    if (VPS_SOK == retVal)
    {
        if (TRUE == streamObj->isModReq[VCORE_VIP_CSC_IDX])
        {
            if (NULL != descMem)
            {
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + streamObj->ovlyOffset[VCORE_VIP_CSC_IDX]);
            }
            retVal = VpsHal_cscSetConfig(
                         instObj->halHandle[VCORE_VIP_CSC_IDX],
                         &streamObj->cscHalCfg,
                         cfgOvlyPtr);
            if (VPS_SOK != retVal)
            {
                GT_0trace(VipCoreTrace, GT_ERR,
                    "CSC HAL Create Overlay Failed!!\n");
            }
        }
    }

    /* Configure the VIP muxes based on the need */
    if (VPS_SOK == retVal)
    {
        if (NULL != descMem)
        {
            /* Assuming here that there are no other non-shadow
               configuration */
            cfgOvlyPtr = (Void *) (descMem->nonShadowOvlyMem);

            retVal = Vcore_vipCreateConfigOvly(&(hObj->resObj), cfgOvlyPtr);
        }
        else
        {
            cfgOvlyPtr = NULL;
        }

        if (VPS_SOK == retVal)
        {
            retVal = Vcore_vipSetMux(&(hObj->resObj), cfgOvlyPtr);
        }
    }

    return (retVal);
}



/**
 *  Vcore_vipProgramScCoeff
 *  \brief Programs the coefficient overlay memory with the scalar coefficients
 *  according to the parameter passed. The upper driver has to submit the
 *  programmed overlay to the VPDMA to do the actual coefficient programming.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param coeffPrms    Pointer to the coeff information.
 *                      This parameter should be non-NULL. The coefficient
 *                      overlay pointers are returned in this structure.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipProgramScCoeff(Vcore_Handle handle,
                                     UInt32 chNum,
                                     Vcore_ScCoeffParams *coeffPrms)
{
    Int32                   retVal = VPS_SOK;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;
    Vcore_VipInstObj       *instObj;
    VpsHal_ScCoeffConfig    coeffCfg;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != coeffPrms));
    hObj = (Vcore_VipHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before programming the overlay memory!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE ==
            chObj->streamObj[VCORE_VIP_STREAMID].isModReq[VCORE_VIP_SC_IDX])
        {
            if (NULL != coeffPrms->userCoeffPtr)
            {
                retVal = VpsHal_scCreateUserCoeffOvly(
                             instObj->halHandle[VCORE_VIP_SC_IDX],
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
                             instObj->halHandle[VCORE_VIP_SC_IDX],
                             &coeffCfg,
                             &(coeffPrms->horzCoeffMemPtr),
                             &(coeffPrms->vertCoeffMemPtr),
                             &(coeffPrms->vertBilinearCoeffMemPtr));
            }
        }
        else
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Scalar is not present to be programmed!!\n");
            retVal = VPS_EFAIL;
        }
    }

    return (retVal);
}



/**
 *  Vcore_vipUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       VIP core handle.
 *  \param descMem      Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frame        Pointer to the FVID frame containing the buffer address.
 *                      This parameter should be non-NULL.
 *  \param fid          FID to be programmed in the descriptor.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  cnt;
    Void                   *descPtr;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;
    Vcore_VipStreamObj     *streamObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != descMem));
    GT_assert(VipCoreTrace, (NULL != frame));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (frame->channelNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, frame->channelNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
        /* Get the Stream object from the channel object */
        streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);
    }

    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < streamObj->numDesc; cnt++)
        {
            descPtr = descMem->outDataDesc[cnt];
            GT_assert(VipCoreTrace, (NULL != descPtr));
            VpsHal_vpdmaSetAddress(
                descPtr,
                0u,             /* FID is reserved for outbound descriptors */
                frame->addr[fid][streamObj->bufIndex[cnt]]);

            /* For outbound descriptors if frame address is NULL,
             * drop the data - don't return error */
            if (NULL == frame->addr[fid][streamObj->bufIndex[cnt]])
            {
                VpsHal_vpdmaSetDropData(descPtr, TRUE);

                /* Workaround: Drop data and Mode bit can not both be set.
                 * So clear memType when setting drop data */
                VpsHal_vpdmaSetMemType(descPtr, VPSHAL_VPDMA_MT_NONTILEDMEM);
            }
            else
            {
                VpsHal_vpdmaSetDropData(descPtr, FALSE);

                /* Set memory type back to original because of above
                 * workaround */
                VpsHal_vpdmaSetMemType(
                    descPtr,
                    (VpsHal_VpdmaMemoryType) streamObj->corePrms.memType);
            }
        }
    }

    return (retVal);
}



/**
 *  Vcore_vipUpdateRtMem
 *  \brief Updates the VIP runtime parameters in the descriptor as well as
 *  in register overlay memories.
 *
 *  \param handle       VIP Path Core handle.
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
 *  \param rtPrms       Pointer to the Vcore_VipRtParams structure
 *                      containing the run time parameters.
 *                      If this parameter is NULL, then the run time
 *                      configuration is updated in the overlay/descriptor
 *                      memories using the previously set configuration.
 *                      Otherwise the run time configurations are updated in
 *                      the channel object as well as updated in overlay and
 *                      descriptor memories.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *  \param frmIdx       [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_vipUpdateRtMem(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  UInt32 chNum,
                                  const Void *rtPrms,
                                  UInt32 frmIdx)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      cnt;
    Void                       *descPtr;
    Void                       *cfgOvlyPtr;
    Vcore_VipHandleObj         *hObj;
    Vcore_VipChObj             *chObj;
    Vcore_VipInstObj           *instObj;
    VpsHal_ScConfig            *scHalCfg;
    Vcore_VipParams            *corePrms;
    const Vcore_VipRtParams    *vipRtPrms;
    Vcore_VipStreamObj         *streamObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != descMem));
    hObj = (Vcore_VipHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
        /* Get the Stream object from the channel object */
        streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);
    }

    if ((VPS_SOK == retVal) && (TRUE == streamObj->isModReq[VCORE_VIP_SC_IDX]))
    {
        /* Check if overlay pointers are non-NULL for the required overlay */
        GT_assert(VipCoreTrace,
            !((0u != streamObj->descInfo.shadowOvlySize)
            && (NULL == descMem->shadowOvlyMem)));

        /* First update the configuration if present */
        scHalCfg = &streamObj->scHalCfg;
        corePrms = &streamObj->corePrms;
        if (NULL != rtPrms)
        {
            vipRtPrms = (const Vcore_VipRtParams *) rtPrms;

            /* Copy the in frame params to core params */
            if (NULL != vipRtPrms->inFrmPrms)
            {
                corePrms->srcWidth = vipRtPrms->inFrmPrms->width;
                if (TRUE == vipRtPrms->isDeinterlacing)
                {
                    /* Since the input to VIP is after the DEI, when
                     * deinterlacing is enabled, the effective height to
                     * VIP will be twice that of input. */
                    corePrms->srcHeight = vipRtPrms->inFrmPrms->height * 2u;
                }
                else
                {
                    corePrms->srcHeight = vipRtPrms->inFrmPrms->height;
                }
            }

            /* Copy the out frame params to core params */
            if (NULL != vipRtPrms->outFrmPrms)
            {
                corePrms->fmt.width = vipRtPrms->outFrmPrms->width;
                corePrms->fmt.height = vipRtPrms->outFrmPrms->height;
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    corePrms->fmt.pitch[cnt] =
                        vipRtPrms->outFrmPrms->pitch[cnt];
                }
                corePrms->memType = vipRtPrms->outFrmPrms->memType;
            }

            /* Copy the crop config to core params */
            if (NULL != vipRtPrms->scCropCfg)
            {
                corePrms->cropCfg.cropStartX = vipRtPrms->scCropCfg->cropStartX;
                corePrms->cropCfg.cropStartY = vipRtPrms->scCropCfg->cropStartY;
                corePrms->cropCfg.cropWidth = vipRtPrms->scCropCfg->cropWidth;
                corePrms->cropCfg.cropHeight = vipRtPrms->scCropCfg->cropHeight;
            }
            else if (NULL != vipRtPrms->inFrmPrms)
            {
                corePrms->cropCfg.cropStartX = 0u;
                corePrms->cropCfg.cropStartY = 0u;
                corePrms->cropCfg.cropWidth = corePrms->srcWidth;
                corePrms->cropCfg.cropHeight = corePrms->srcHeight;
            }

            /* Copy the scalar runtime configuraton to core params */
            if (NULL != vipRtPrms->scRtCfg)
            {
                corePrms->scCfg.bypass = vipRtPrms->scRtCfg->scBypass;

                /* Update scalar phase info from application provided data*/
                if (TRUE == vipRtPrms->scRtCfg->scSetPhInfo)
                {
                    scHalCfg->phInfoMode = VPS_SC_SET_PHASE_INFO_FROM_APP;
                    scHalCfg->rowAccInc = vipRtPrms->scRtCfg->rowAccInc;
                    scHalCfg->rowAccOffset = vipRtPrms->scRtCfg->rowAccOffset;
                    scHalCfg->rowAccOffsetB = vipRtPrms->scRtCfg->rowAccOffsetB;
                    scHalCfg->ravScFactor = vipRtPrms->scRtCfg->ravScFactor;
                    scHalCfg->ravRowAccInit = vipRtPrms->scRtCfg->ravRowAccInit;
                    scHalCfg->ravRowAccInitB =
                        vipRtPrms->scRtCfg->ravRowAccInitB;

                scHalCfg->linAccIncr     =  vipRtPrms->scRtCfg->linAccIncr;
                scHalCfg->colAccOffset   =  vipRtPrms->scRtCfg->colAccOffset;
                }
            }

            /* Copy it to HAL configuration */
            scHalCfg->srcWidth = corePrms->srcWidth;
            scHalCfg->srcHeight = corePrms->srcHeight;
            scHalCfg->tarWidth = corePrms->fmt.width;
            scHalCfg->tarHeight = corePrms->fmt.height;
            scHalCfg->cropWidth = corePrms->cropCfg.cropWidth;
            scHalCfg->cropHeight = corePrms->cropCfg.cropHeight;
            scHalCfg->cropStartX = corePrms->cropCfg.cropStartX;
            scHalCfg->cropStartY = corePrms->cropCfg.cropStartY;
            scHalCfg->bypass = corePrms->scCfg.bypass;
        }

        /* Program the overlay memory */
        cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
            + streamObj->ovlyOffset[VCORE_VIP_SC_IDX]);
        retVal = VpsHal_scSetConfig(
                     instObj->halHandle[VCORE_VIP_SC_IDX],
                     scHalCfg,
                     cfgOvlyPtr);
        if (VPS_SOK != retVal)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Runtime SCALAR HAL Config Overlay Failed!!\n");
        }
    }

    /* Update out frame parameters irrespective of whether scalar is present
     * or not as they are part of descriptors. */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < streamObj->numDesc; cnt++)
        {
            descPtr = descMem->outDataDesc[cnt];
            GT_assert(VipCoreTrace, (NULL != descPtr));

            /* Update pitch and memory type in VPDMA descriptor memory */
            VpsHal_vpdmaSetLineStride(
                descPtr,
                streamObj->corePrms.fmt.pitch[streamObj->bufIndex[cnt]]);
            VpsHal_vpdmaSetMemType(
                descPtr,
                (VpsHal_VpdmaMemoryType) streamObj->corePrms.memType);
        }

    }

    return (retVal);
}



/**
 *  Vcore_vipSetFsEvent
 *  \brief Program the client's frame start.
 *
 *  \param handle       VIP core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param fsEvent      Frame start event.
 *  \param descMem      Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actual registers. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *
 *  \return             Returns 0 on success else returns error value.
 */
// Todo: Update the overlay memory when descMem is non-NULL and update actual
// register when descMem is NULL.
static Int32 Vcore_vipSetFsEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt;
    Vcore_VipHandleObj     *hObj;
    Vcore_VipChObj         *chObj;
    Vcore_VipStreamObj     *streamObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != handle));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Set the parameter before setting the frame start event!!\n");
            retVal = VPS_EFAIL;
        }
        /* Get the Stream object from the channel object */
        streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);
    }

    if (VPS_SOK == retVal)
    {
        for (chCnt = 0u; chCnt < streamObj->numDesc; chCnt++)
        {
            /* Program the frame start event */
            VpsHal_vpdmaSetFrameStartEvent(
                streamObj->vpdmaCh[chCnt],
                fsEvent,
                VPSHAL_VPDMA_LM_0,
                0u,
                NULL,
                0u);
        }
    }

    return (retVal);
}


/**
 *  Vcore_vipGetScFactorConfig
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
static Int32 Vcore_vipGetScFactorConfig(Vcore_Handle handle,
                                        UInt32 chNum,
                                        VpsHal_ScFactorConfig *scFactorConfig,
                                        const Vcore_DescMem *descMem)
{
    Int32 retVal = VPS_SOK;
    Void *cfgOvlyPtr = NULL;
    Vcore_VipHandleObj *hObj;
    Vcore_VipChObj *chObj;
    VpsHal_ScConfig *scHalCfg;

    /* Check for errors */
    GT_assert(VipCoreTrace, (NULL != handle));
    GT_assert(VipCoreTrace, (NULL != scFactorConfig));
    GT_assert(VipCoreTrace, (NULL != descMem));
    GT_assert(VipCoreTrace, (NULL != descMem->shadowOvlyMem));

    hObj = (Vcore_VipHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    else
    {
        chObj = vcoreVipGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(VipCoreTrace, (NULL != chObj));

        if (TRUE ==
            chObj->streamObj[VCORE_VIP_STREAMID].isModReq[VCORE_VIP_SC_IDX])
        {
            /* Call the HAL function to get the scaling factor config. */
            scHalCfg = &(chObj->streamObj[VCORE_VIP_STREAMID].scHalCfg);
            cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                + chObj->streamObj->ovlyOffset[VCORE_VIP_SC_IDX]);
            retVal = VpsHal_scGetScFactorConfig(
                        hObj->parent->halHandle[VCORE_VIP_SC_IDX],
                        scHalCfg,
                        scFactorConfig,
                        cfgOvlyPtr);
        }
        else
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "Scalar is not present to get the scaling factor!!\n");
            retVal = VPS_EFAIL;
        }
    }

    return (retVal);
}



/*------------------------------------------------------------------------------
 * Internal functions
 *------------------------------------------------------------------------------
 */

/**
 *  vcoreVipFigureOutState
 *  Figure out the different VIP parameters based on given input parameters.
 */
static Int32 vcoreVipFigureOutState(const Vcore_VipHandleObj *hObj,
                                    Vcore_VipStreamObj *streamObj)
{
    Int32                        retVal = VPS_SOK;
    UInt32                       chCnt;
    Vcore_VipParams             *corePrms;
    const Vcore_VipVpdmaInfo    *vpdmaInfo;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != streamObj));

    corePrms = &(streamObj->corePrms);

    if ((FALSE == corePrms->isScReq) && (FALSE == corePrms->scCfg.bypass))
    {
        retVal = VPS_EBADARGS;
    }

    if (VPS_SOK == retVal)
    {
        streamObj->isModReq[VCORE_VIP_SC_IDX] = FALSE;
        streamObj->isModReq[VCORE_VIP_CSC_IDX] = FALSE;
        streamObj->isModReq[VCORE_VIP_CHR_DS_IDX] = FALSE;

        if (TRUE == streamObj->corePrms.isScReq)
        {
            streamObj->isModReq[VCORE_VIP_SC_IDX] = TRUE;
        }
        if (-1 != hObj->resObj.muxInfo.cscSrc)
        {
            streamObj->isModReq[VCORE_VIP_CSC_IDX] = TRUE;
        }
        if ((-1 != hObj->resObj.muxInfo.chrDsBypass[0u]) ||
            (-1 != hObj->resObj.muxInfo.chrDsBypass[1u]))
        {
            streamObj->isModReq[VCORE_VIP_CHR_DS_IDX] = TRUE;
        }

        vpdmaInfo = &(hObj->resObj.vpdmaInfo[streamObj->streamId]);
        for (chCnt = 0u; chCnt < vpdmaInfo->vpdmaChNum; chCnt++)
        {
            streamObj->vpdmaCh[chCnt] = vpdmaInfo->vpdmaChId[chCnt];
            streamObj->vpdmaChDataType[chCnt] =
                vpdmaInfo->vpdmaChDataType[chCnt];
            streamObj->descInfo.socChNum[chCnt] =
                vpdmaInfo->vpdmaChId[chCnt];
        }

        /* Set the Buffer index based on the dataFormat */
        if (FVID2_DF_RGB24_888 == corePrms->fmt.dataFormat)
        {
            streamObj->bufIndex[VCORE_VIP_RGB_IDX] = FVID2_RGB_ADDR_IDX;
        }
        else if (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
        {
            /* YUV 420 semi-planar format */
            streamObj->bufIndex[VCORE_VIP_Y_IDX] = FVID2_YUV_SP_Y_ADDR_IDX;
            streamObj->bufIndex[VCORE_VIP_CBCR_IDX] = FVID2_YUV_SP_CBCR_ADDR_IDX;
        }
        else
        {
            /* YUV 422 interleaved format - C buffer is also same as Y */
            streamObj->bufIndex[VCORE_VIP_Y_IDX] = FVID2_YUV_INT_ADDR_IDX;
            streamObj->bufIndex[VCORE_VIP_CBCR_IDX] = FVID2_YUV_INT_ADDR_IDX;
        }
        streamObj->numDesc = vpdmaInfo->vpdmaChNum;
        streamObj->descInfo.numChannels = vpdmaInfo->vpdmaChNum;
    }
    return (retVal);
}



/**
 *  vcoreVipSetScHalCfg
 *  Sets the Scalar HAL configuration based on params set.
 */
static Int32 vcoreVipSetScHalCfg(Vcore_VipStreamObj *streamObj)
{
    Int32                   retVal = VPS_SOK;
    VpsHal_ScConfig        *scHalCfg;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != streamObj));

    scHalCfg = &(streamObj->scHalCfg);
    scHalCfg->srcWidth = streamObj->corePrms.srcWidth;
    scHalCfg->srcHeight = streamObj->corePrms.srcHeight;
    scHalCfg->tarWidth = streamObj->corePrms.fmt.width;
    scHalCfg->tarHeight = streamObj->corePrms.fmt.height;
    scHalCfg->cropStartX = streamObj->corePrms.cropCfg.cropStartX;
    scHalCfg->cropStartY = streamObj->corePrms.cropCfg.cropStartY;
    scHalCfg->cropWidth = streamObj->corePrms.cropCfg.cropWidth;
    scHalCfg->cropHeight = streamObj->corePrms.cropCfg.cropHeight;
    if (FVID2_SF_INTERLACED == streamObj->corePrms.fmt.scanFormat)
    {
        scHalCfg->inFrameMode = VPS_SF_INTERLACED;
    }
    else
    {
        scHalCfg->inFrameMode = VPS_SF_PROGRESSIVE;
    }

    if (FVID2_SF_INTERLACED == streamObj->corePrms.fmt.scanFormat)
    {
        scHalCfg->outFrameMode = VPS_SF_INTERLACED;
    }
    else
    {
        scHalCfg->outFrameMode = VPS_SF_PROGRESSIVE;
    }

    scHalCfg->hsType = VPS_SC_HST_AUTO;
    scHalCfg->nonLinear = streamObj->corePrms.scCfg.nonLinear;
    scHalCfg->stripSize = streamObj->corePrms.scCfg.stripSize;
    scHalCfg->vsType = streamObj->corePrms.scCfg.vsType;
    scHalCfg->fidPol = VPS_FIDPOL_NORMAL;
    scHalCfg->selfGenFid = VPS_SC_SELF_GEN_FID_DISABLE;
    scHalCfg->defConfFactor = 0u;
    scHalCfg->biLinIntpType = VPS_SC_BINTP_MODIFIED;
    scHalCfg->enableEdgeDetect = streamObj->corePrms.scCfg.enableEdgeDetect;
    scHalCfg->hPolyBypass = TRUE;
    scHalCfg->bypass = streamObj->corePrms.scCfg.bypass;
    scHalCfg->enablePeaking = streamObj->corePrms.scCfg.enablePeaking;
    scHalCfg->phInfoMode = VPS_SC_SET_PHASE_INFO_DEFAULT;

    return (retVal);
}



/**
 *  vcoreVipCheckParams
 *  Checks whether a given parameters are valid or not.
 *  Returns 0 if everything is fine else returns error value.
 */
static Int32 vcoreVipCheckParams(const Vcore_VipParams *corePrms)
{
    Int32       retVal = VPS_SOK;

    /* Check whether data format is supported or not */
#ifdef VPS_CAPT_422SP_SUPPORT
    if (!((FVID2_DF_YUV422I_YUYV == corePrms->fmt.dataFormat)
       || (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
       || (FVID2_DF_RGB24_888 == corePrms->fmt.dataFormat)
       || (FVID2_DF_YUV422SP_UV == corePrms->fmt.dataFormat)))
#else
    if (!((FVID2_DF_YUV422I_YUYV == corePrms->fmt.dataFormat)
       || (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
       || (FVID2_DF_RGB24_888 == corePrms->fmt.dataFormat)))
#endif /* VPS_CAPT_422SP_SUPPORT */

    {
        GT_1trace(VipCoreTrace, GT_ERR,
            "Data format (%d) not supported!!\n",
            corePrms->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_DF_YUV422I_YUYV == corePrms->fmt.dataFormat)
    {
        /* Check whether the pitch is valid */
        if (corePrms->fmt.pitch[FVID2_YUV_INT_ADDR_IDX]
            < (corePrms->fmt.width * 2u))
        {
            GT_2trace(VipCoreTrace, GT_ERR,
                "Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_YUV_INT_ADDR_IDX],
                (corePrms->fmt.width * 2u));
            retVal = VPS_EINVALID_PARAMS;
        }
    }

#ifdef VPS_CAPT_422SP_SUPPORT
    if ((FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat) ||
        (FVID2_DF_YUV422SP_UV == corePrms->fmt.dataFormat))
#else
    if (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
#endif
    {
        /* Check whether the pitch is valid */
        if (corePrms->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX]
            < corePrms->fmt.width)
        {
            GT_2trace(VipCoreTrace, GT_ERR,
                "Y Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX],
                corePrms->fmt.width);
            retVal = VPS_EINVALID_PARAMS;
        }
        if (corePrms->fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX]
            < corePrms->fmt.width)
        {
            GT_2trace(VipCoreTrace, GT_ERR,
                "CbCr Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX],
                corePrms->fmt.width);
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    if (FVID2_DF_RGB24_888 == corePrms->fmt.dataFormat)
    {
        /* Check whether the pitch is valid */
        if (corePrms->fmt.pitch[FVID2_RGB_ADDR_IDX]
            < (corePrms->fmt.width * 3u))
        {
            GT_2trace(VipCoreTrace, GT_ERR,
                "Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_RGB_ADDR_IDX],
                (corePrms->fmt.width * 3u));
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    /* Check whether window width is even */
    if (corePrms->fmt.width & 0x01u)
    {
        GT_1trace(VipCoreTrace, GT_ERR,
            "Width(%d) can't be odd!!\n", corePrms->fmt.width);
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check whether window height is even for YUV420 format */
    if (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
    {
        if (corePrms->fmt.height & 0x01u)
        {
            GT_1trace(VipCoreTrace, GT_ERR,
                "Height(%d) can't be odd for YUV420 format!!\n",
                corePrms->fmt.height);
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    return (retVal);
}



/**
 *  vcoreVipProgramDesc
 *  Programs the data descriptor depending on the parameters set.
 *  This is used in case of non-mosaic mode of operation.
 */
static UInt32 vcoreVipProgramDesc(const Vcore_VipHandleObj *hObj,
                                  const Vcore_VipStreamObj *streamObj,
                                  const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      cnt;
    Void                       *descPtr;
    Vcore_VipInstObj           *instObj;
    VpsHal_VpdmaOutDescParams   outDescPrm;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != streamObj));
    GT_assert(VipCoreTrace, (NULL != descMem));
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));

    /* Set the commom VPDMA parameters between all descriptors */
    for (cnt = 0u; cnt < streamObj->numDesc; cnt++)
    {
        outDescPrm.lineStride =
            streamObj->corePrms.fmt.pitch[streamObj->bufIndex[cnt]];
        outDescPrm.notify = FALSE;
        if (VCORE_OPMODE_CAPTURE == instObj->curMode)
        {
            outDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR_VIP;
        }
        else
        {
            outDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR_VIP;
        }
        outDescPrm.memType =
            (VpsHal_VpdmaMemoryType) streamObj->corePrms.memType;
        outDescPrm.maxWidth =
            VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
        outDescPrm.maxHeight =
            VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED;
        outDescPrm.is1DMem = FALSE;
        outDescPrm.channel =
            streamObj->vpdmaCh[streamObj->bufIndex[cnt]];
        outDescPrm.nextChannel =
            streamObj->vpdmaCh[streamObj->bufIndex[cnt]];
        if ((VCORE_OPMODE_CAPTURE == instObj->curMode) &&
            (FVID2_SF_INTERLACED == streamObj->corePrms.fmt.scanFormat) &&
            (TRUE ==
                streamObj->corePrms.fmt.fieldMerged[streamObj->bufIndex[cnt]]))
        {
            outDescPrm.lineSkip = VPSHAL_VPDMA_LS_2;
        }
        else
        {
            outDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
        }

        outDescPrm.dataType =
            streamObj->vpdmaChDataType[streamObj->bufIndex[cnt]];
        /* Configure data descriptor */
        descPtr = descMem->outDataDesc[cnt];
        GT_assert(VipCoreTrace, (NULL != descPtr));
        retVal = VpsHal_vpdmaCreateOutBoundDataDesc(
                    descPtr,
                    &outDescPrm);
        if (VPS_SOK != retVal)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "VPDMA Descriptor Creation Failed!!\n");
                break;
        }
        else
        {
            /* Print the created descriptor memory */
            GT_0trace(VipCoreTrace, GT_DEBUG, "Out Data Descrirptor:\n");
            GT_0trace(VipCoreTrace, GT_DEBUG, "---------------------\n");
            VpsHal_vpdmaPrintDesc(descPtr, VipCoreTrace);
        }
    }

    return (retVal);
}



/**
 *  vcoreVipGetChObj
 *  Returns the channel object pointer depending on per channel config is
 *  enabled or not.
 */
static inline Vcore_VipChObj *vcoreVipGetChObj(Vcore_VipHandleObj *hObj,
                                               UInt32 chNum)
{
    Vcore_VipChObj    *chObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObj));

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



/**
 *  vcoreVipAllocHandleObj
 *  Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
static Vcore_VipHandleObj *vcoreVipAllocHandleObj(UInt32 instId,
                                                  Vcore_OpMode mode)
{
    UInt32              instCnt, hCnt;
    UInt32              cookie;
    Vcore_VipInstObj   *instObj = NULL;
    Vcore_VipHandleObj *hObj = NULL;

    /* Find out the instance to which this handle belongs to */
    for (instCnt = 0u; instCnt < VCORE_VIP_NUM_INST; instCnt++)
    {
        if (VipInstObjects[instCnt].instId == instId)
        {
            instObj = &VipInstObjects[instCnt];
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
            /* Core already opend for some mode. For capture mode, only one
               handle should be allowed. It doesn't make sense isn't it? */
            if ((VCORE_OPMODE_CAPTURE == instObj->curMode)
                || ((VCORE_OPMODE_MEMORY == instObj->curMode)
                    && (VCORE_OPMODE_MEMORY != mode)))
            {
                /* Already opened mode and requested mode are not matching */
                GT_0trace(VipCoreTrace, GT_ERR,
                    "Already opened mode and requested mode not matching!!\n");
                instObj = NULL;
            }
        }
    }

    if (NULL != instObj)
    {
        /* Use a free handle object */
        for (hCnt = 0u; hCnt < instObj->numHandle; hCnt++)
        {
            if (FALSE == instObj->hObjs[hCnt].isUsed)
            {
                hObj = &instObj->hObjs[hCnt];
                break;
            }
        }
    }

    if (NULL != hObj)
    {
        /* Update the state and return the handle handle */
        hObj->isUsed = TRUE;
        hObj->parent = instObj;
        instObj->openCnt++;
        instObj->curMode = mode;
        vcoreVipSetDefChInfo(&hObj->handleContext, 0u);
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);

    return (hObj);
}



/**
 *  vcoreVipFreeHandleObj
 *  Frees-up the handle object and resets the variables.
 */
static Int32 vcoreVipFreeHandleObj(Vcore_VipHandleObj *hObj)
{
    UInt32              hCnt;
    UInt32              cookie;
    Int32               retVal = VPS_EFAIL;
    Vcore_VipInstObj   *instObj;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Free the handle object */
    for (hCnt = 0u; hCnt < instObj->numHandle; hCnt++)
    {
        if (hObj == &instObj->hObjs[hCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(VipCoreTrace, (TRUE == instObj->hObjs[hCnt].isUsed));

            /* Reset the variables */
            hObj->isUsed = FALSE;
            hObj->parent = NULL;
            hObj->perChCfg = FALSE;
            hObj->numCh = 0u;
            vcoreVipSetDefChInfo(&hObj->handleContext, 0u);

            /* If last handle of the instance, then set mode to invalid */
            if (instObj->openCnt > 0u)
            {
                instObj->openCnt--;
            }
            if (0u == instObj->openCnt)
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
 *  vcoreVipSetDefChInfo
 *  Resets the channel information with default values.
 */
static void vcoreVipSetDefChInfo(Vcore_VipChObj *chObj, UInt32 chNum)
{
    UInt32              cnt, streamCnt;
    Vcore_VipStreamObj     *stream;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isParamsSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    for (streamCnt = 0u; streamCnt < VCORE_VIP_MAX_STREAMS; streamCnt++)
    {
        stream = &(chObj->streamObj[streamCnt]);

        /* Initialize core parameters - Assumes YUV422 720x480 progressive */
        stream->streamId = streamCnt;
        stream->corePrms.memType =
            (Vps_VpdmaMemoryType) VPSHAL_VPDMA_MT_NONTILEDMEM;

        stream->corePrms.fmt.channelNum = chNum;
        stream->corePrms.fmt.width = VCORE_VIP_DEFAULT_WIDTH;
        stream->corePrms.fmt.height = VCORE_VIP_DEFAULT_HEIGHT;
        stream->corePrms.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
            VCORE_VIP_DEFAULT_WIDTH * 2u;
        stream->corePrms.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
        stream->corePrms.fmt.dataFormat = FVID2_DF_YUV422I_YUYV;
        stream->corePrms.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
        stream->corePrms.fmt.bpp = FVID2_BPP_BITS16;
        stream->corePrms.fmt.reserved = NULL;

        /* Initialize SC parameters */
        stream->corePrms.scCfg.bypass = TRUE;
        stream->corePrms.cropCfg.cropStartY = 0u;
        stream->corePrms.cropCfg.cropStartX = 0u;
        stream->corePrms.cropCfg.cropWidth = VCORE_VIP_DEFAULT_WIDTH;
        stream->corePrms.cropCfg.cropHeight = VCORE_VIP_DEFAULT_HEIGHT;
        stream->corePrms.scCfg.nonLinear = FALSE;
        stream->corePrms.scCfg.stripSize = 0u;
        stream->corePrms.scCfg.vsType = VPS_SC_VST_POLYPHASE;
        stream->corePrms.srcWidth = VCORE_VIP_DEFAULT_WIDTH;
        stream->corePrms.srcHeight = VCORE_VIP_DEFAULT_HEIGHT;

        /* Initialize channel info structure */
        stream->descInfo.numInDataDesc = 0u;
        stream->descInfo.numOutDataDesc = 0u;
        stream->descInfo.numMultiWinDataDesc = 0u;
        stream->descInfo.shadowOvlySize = 0u;
        stream->descInfo.nonShadowOvlySize = 0u;
        stream->descInfo.horzCoeffOvlySize = 0u;
        stream->descInfo.vertCoeffOvlySize = 0u;
        stream->descInfo.vertBilinearCoeffOvlySize = 0u;

        stream->numDesc = 1u;
        for (cnt = 0u; cnt < VCORE_VIP_MAX_MOD; cnt++)
        {
            stream->isModReq[cnt] = FALSE;
        }
        for (cnt = 0u; cnt < VCORE_VIP_MAX_HAL; cnt++)
        {
            stream->ovlyOffset[cnt] = 0u;
        }
    }

    return;
}



/**
 *  vcoreVipAllocChObj
 *  Allocates channel objects.
 *  Returns error if allocation failed.
 */
static Int32 vcoreVipAllocChObj(Vcore_VipChObj **chObjs, UInt32 numCh)
{
    Int32       retVal = VPS_SOK;
    UInt32      chCnt, errCnt;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != chObjs));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Allocate channel objects one at a time */
        chObjs[chCnt] = vcoreVipAllocChMem();
        if (NULL == chObjs[chCnt])
        {
            GT_1trace(VipCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                chCnt);
            retVal = VPS_EALLOC;
            break;
        }

        /* Set default values for the channel object */
        vcoreVipSetDefChInfo(chObjs[chCnt], chCnt);
    }

    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        for (errCnt = 0u; errCnt < chCnt; errCnt++)
        {
            vcoreVipFreeChMem(chObjs[errCnt]);
            chObjs[errCnt] = NULL;
        }
    }

    return (retVal);
}



/**
 *  vcoreVipFreeChObj
 *  Frees-up the channel objects.
 */
static Int32 vcoreVipFreeChObj(Vcore_VipChObj **chObjs, UInt32 numCh)
{
    UInt32          chCnt;
    Int32           retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != chObjs));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Memset the channel object before freeing */
        VpsUtils_memset(chObjs[chCnt], 0u, sizeof(Vcore_VipChObj));
        retVal |= vcoreVipFreeChMem(chObjs[chCnt]);
        chObjs[chCnt] = NULL;
    }

    return (retVal);
}



/**
 *  vcoreVipAllocHandleMem
 *  Allocate memory for handle objects from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_VipHandleObj *vcoreVipAllocHandleMem(UInt32 numHandle)
{
    UInt32              instCnt;
    Vcore_VipHandleObj *hObjs = NULL;

    /* Check if the requested number of channels is within range */
    GT_assert(VipCoreTrace, (numHandle <= VCORE_VIP_MAX_HANDLES));

    Semaphore_pend(VipPoolSem, VCORE_VIP_SEM_TIMEOUT);

    for (instCnt = 0u; instCnt < VCORE_VIP_NUM_INST; instCnt++)
    {
        if (FALSE == VipHandleMemFlag[instCnt])
        {
            hObjs = &VipHandleMemPool[instCnt][0u];
            VipHandleMemFlag[instCnt] = TRUE;
            break;
        }
    }

    if (NULL == hObjs)
    {
        GT_0trace(VipCoreTrace, GT_ERR,
            "Channel Object memory alloc failed!!\n");
    }

    Semaphore_post(VipPoolSem);

    return (hObjs);
}



/**
 *  vcoreVipFreeHandleMem
 *  Frees-up the handle objects memory.
 */
static Int32 vcoreVipFreeHandleMem(const Vcore_VipHandleObj *hObjs)
{
    UInt32              instCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObjs));

    Semaphore_pend(VipPoolSem, VCORE_VIP_SEM_TIMEOUT);

    for (instCnt = 0u; instCnt < VCORE_VIP_NUM_INST; instCnt++)
    {
        if (&VipHandleMemPool[instCnt][0u] == hObjs)
        {
            /* Check if the memory is already allocated */
            GT_assert(VipCoreTrace, (TRUE == VipHandleMemFlag[instCnt]));
            VipHandleMemFlag[instCnt] = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }

    Semaphore_post(VipPoolSem);

    return (retVal);
}



/**
 *  vcoreVipAllocChMem
 *  Allocate memory for channel object from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_VipChObj *vcoreVipAllocChMem(void)
{
    UInt32              chCnt;
    Vcore_VipChObj     *chObj = NULL;

    Semaphore_pend(VipPoolSem, VCORE_VIP_SEM_TIMEOUT);

    for (chCnt = 0u; chCnt < VCORE_VIP_MAX_CHANNEL_OBJECTS; chCnt++)
    {
        if (FALSE == VipChMemFlag[chCnt])
        {
            chObj = &VipChMemPool[chCnt];
            VipChMemFlag[chCnt] = TRUE;
            break;
        }
    }

    if (NULL == chObj)
    {
        GT_0trace(VipCoreTrace, GT_ERR,
            "Channel Object memory alloc failed!!\n");
    }

    Semaphore_post(VipPoolSem);

    return (chObj);
}



/**
 *  vcoreVipFreeChMem
 *  Frees-up the channel object memory.
 */
static Int32 vcoreVipFreeChMem(const Vcore_VipChObj *chObj)
{
    UInt32          chCnt;
    Int32           retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != chObj));

    Semaphore_pend(VipPoolSem, VCORE_VIP_SEM_TIMEOUT);

    for (chCnt = 0u; chCnt < VCORE_VIP_MAX_CHANNEL_OBJECTS; chCnt++)
    {
        if (&VipChMemPool[chCnt] == chObj)
        {
            /* Check if the memory is already allocated */
            GT_assert(VipCoreTrace, (TRUE == VipChMemFlag[chCnt]));
            VipChMemFlag[chCnt] = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }

    Semaphore_post(VipPoolSem);

    return (retVal);
}



/**
 * Initialize Resource Manager Object
 */
static Void vcoreInitResMngrObj(Vcore_VipHandleObj *hObj)
{
    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != hObj->parent));

    /* Opening Resource Manager in Shared mode as there could
       be multiple handles for the same driver */
    hObj->resAllocParams.openMode = VCORE_VIP_RES_ALLOC_SHARED;
    hObj->resAllocParams.drvName = VCORE_VIP_CORE_NAME;
    hObj->resAllocParams.drvId = hObj->parent->instId;
    hObj->resAllocParams.vipInstId = hObj->parent->vipInst;
    hObj->resAllocParams.inSrc = hObj->parent->vipInSrc;
    hObj->resAllocParams.numOutParams = hObj->parent->numVipOutSteams;
    hObj->resAllocParams.muxModeStartChId = 0u;
}



static Int32 vcoreVipScSetAdvConfig(Vcore_VipHandleObj *hObj,
                                    const Vcore_ScCfgParams *scCfgPrms)
{
    Int32                   retVal = VPS_EFAIL;
    Void                   *cfgOvlyPtr = NULL;
    Vcore_VipChObj         *chObj;
    Vcore_VipInstObj       *instObj;
    Vcore_VipStreamObj     *streamObj;
    VpsHal_ScConfig         scHalCfg;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != scCfgPrms));
    GT_assert(VipCoreTrace, (NULL != scCfgPrms->scAdvCfg));
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));
    chObj = vcoreVipGetChObj(hObj, scCfgPrms->chNum);
    GT_assert(VipCoreTrace, (NULL != chObj));
    streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);

    if ((TRUE == streamObj->isModReq[VCORE_VIP_SC_IDX]) &&
        (NULL != instObj->halHandle[VCORE_VIP_SC_IDX]))
    {
        /* Copy the configuration */
        scHalCfg.tarWidth = scCfgPrms->scAdvCfg->tarWidth;
        scHalCfg.tarHeight = scCfgPrms->scAdvCfg->tarHeight;
        scHalCfg.srcWidth = scCfgPrms->scAdvCfg->srcWidth;
        scHalCfg.srcHeight = scCfgPrms->scAdvCfg->srcHeight;
        scHalCfg.cropStartX = scCfgPrms->scAdvCfg->cropStartX;
        scHalCfg.cropStartY = scCfgPrms->scAdvCfg->cropStartY;
        scHalCfg.cropWidth = scCfgPrms->scAdvCfg->cropWidth;
        scHalCfg.cropHeight = scCfgPrms->scAdvCfg->cropHeight;
        scHalCfg.inFrameMode =
            (Vps_ScanFormat) scCfgPrms->scAdvCfg->inFrameMode;
        scHalCfg.outFrameMode =
            (Vps_ScanFormat) scCfgPrms->scAdvCfg->outFrameMode;
        scHalCfg.hsType = scCfgPrms->scAdvCfg->hsType;
        scHalCfg.nonLinear = scCfgPrms->scAdvCfg->nonLinear;
        scHalCfg.stripSize = scCfgPrms->scAdvCfg->stripSize;
        scHalCfg.vsType = scCfgPrms->scAdvCfg->vsType;
        scHalCfg.fidPol = (Vps_FidPol) scCfgPrms->scAdvCfg->fidPol;
        scHalCfg.selfGenFid =
            (Vps_ScSelfGenFid) scCfgPrms->scAdvCfg->selfGenFid;
        scHalCfg.defConfFactor = scCfgPrms->scAdvCfg->defConfFactor;
        scHalCfg.biLinIntpType = scCfgPrms->scAdvCfg->biLinIntpType;
        scHalCfg.enableEdgeDetect = scCfgPrms->scAdvCfg->enableEdgeDetect;
        scHalCfg.hPolyBypass = scCfgPrms->scAdvCfg->hPolyBypass;
        scHalCfg.enablePeaking = scCfgPrms->scAdvCfg->enablePeaking;
        scHalCfg.bypass = scCfgPrms->scAdvCfg->bypass;
        scHalCfg.phInfoMode = VPS_SC_SET_PHASE_INFO_FROM_APP;
        scHalCfg.rowAccInc = scCfgPrms->scAdvCfg->rowAccInc;
        scHalCfg.rowAccOffset = scCfgPrms->scAdvCfg->rowAccOffset;
        scHalCfg.rowAccOffsetB = scCfgPrms->scAdvCfg->rowAccOffsetB;
        scHalCfg.ravScFactor = scCfgPrms->scAdvCfg->ravScFactor;
        scHalCfg.ravRowAccInit = scCfgPrms->scAdvCfg->ravRowAccInit;
        scHalCfg.ravRowAccInitB = scCfgPrms->scAdvCfg->ravRowAccInitB;
        scHalCfg.peakingCfg = scCfgPrms->scAdvCfg->peakingCfg;
        scHalCfg.edgeDetectCfg = scCfgPrms->scAdvCfg->edgeDetectCfg;

        cfgOvlyPtr = NULL;
        if (NULL != scCfgPrms->descMem)
        {
            cfgOvlyPtr = (Void *) ((UInt32) scCfgPrms->descMem->shadowOvlyMem
                + streamObj->ovlyOffset[VCORE_VIP_SC_IDX]);
        }

        /* Set the scalar configuration to HAL */
        retVal = VpsHal_scSetAdvConfig(
                     instObj->halHandle[VCORE_VIP_SC_IDX],
                     &scHalCfg,
                     cfgOvlyPtr);
        if (VPS_SOK != retVal)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "SCALAR HAL Set Config Overlay Failed!!\n");
        }
    }

    return (retVal);
}



static Int32 vcoreVipScGetAdvConfig(Vcore_VipHandleObj *hObj,
                                    Vcore_ScCfgParams *scCfgPrms)
{
    Int32                   retVal = VPS_EFAIL;
    Void                   *cfgOvlyPtr = NULL;
    Vcore_VipChObj         *chObj;
    Vcore_VipInstObj       *instObj;
    Vcore_VipStreamObj     *streamObj;
    VpsHal_ScConfig         scHalCfg;
    Vps_ScPeakingConfig     peakingCfg;
    Vps_ScEdgeDetectConfig  edgeDetectCfg;

    /* NULL pointer check */
    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != scCfgPrms));
    GT_assert(VipCoreTrace, (NULL != scCfgPrms->scAdvCfg));
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));
    chObj = vcoreVipGetChObj(hObj, scCfgPrms->chNum);
    GT_assert(VipCoreTrace, (NULL != chObj));
    streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);

    if ((TRUE == streamObj->isModReq[VCORE_VIP_SC_IDX]) &&
        (NULL != instObj->halHandle[VCORE_VIP_SC_IDX]))
    {
        cfgOvlyPtr = NULL;
        if (NULL != scCfgPrms->descMem)
        {
            cfgOvlyPtr = (Void *) ((UInt32) scCfgPrms->descMem->shadowOvlyMem
                + streamObj->ovlyOffset[VCORE_VIP_SC_IDX]);
        }

        /* Get the scalar configuration from HAL */
        scHalCfg.peakingCfg = &peakingCfg;
        scHalCfg.edgeDetectCfg = &edgeDetectCfg;
        retVal = VpsHal_scGetConfig(
                     instObj->halHandle[VCORE_VIP_SC_IDX],
                     &scHalCfg,
                     cfgOvlyPtr);
        if (VPS_SOK != retVal)
        {
            GT_0trace(VipCoreTrace, GT_ERR,
                "SCALAR HAL Get Config Overlay Failed!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Copy the configuration */
        scCfgPrms->scAdvCfg->tarWidth = scHalCfg.tarWidth;
        scCfgPrms->scAdvCfg->tarHeight = scHalCfg.tarHeight;
        scCfgPrms->scAdvCfg->srcWidth = scHalCfg.srcWidth;
        scCfgPrms->scAdvCfg->srcHeight = scHalCfg.srcHeight;
        scCfgPrms->scAdvCfg->cropStartX = scHalCfg.cropStartX;
        scCfgPrms->scAdvCfg->cropStartY = scHalCfg.cropStartY;
        scCfgPrms->scAdvCfg->cropWidth = scHalCfg.cropWidth;
        scCfgPrms->scAdvCfg->cropHeight = scHalCfg.cropHeight;
        scCfgPrms->scAdvCfg->inFrameMode = scHalCfg.inFrameMode;
        scCfgPrms->scAdvCfg->outFrameMode = scHalCfg.outFrameMode;
        scCfgPrms->scAdvCfg->hsType = scHalCfg.hsType;
        scCfgPrms->scAdvCfg->nonLinear = scHalCfg.nonLinear;
        scCfgPrms->scAdvCfg->stripSize = scHalCfg.stripSize;
        scCfgPrms->scAdvCfg->vsType = scHalCfg.vsType;
        scCfgPrms->scAdvCfg->fidPol = scHalCfg.fidPol;
        scCfgPrms->scAdvCfg->selfGenFid = scHalCfg.selfGenFid;
        scCfgPrms->scAdvCfg->defConfFactor = scHalCfg.defConfFactor;
        scCfgPrms->scAdvCfg->biLinIntpType = scHalCfg.biLinIntpType;
        scCfgPrms->scAdvCfg->enableEdgeDetect = scHalCfg.enableEdgeDetect;
        scCfgPrms->scAdvCfg->hPolyBypass = scHalCfg.hPolyBypass;
        scCfgPrms->scAdvCfg->enablePeaking = scHalCfg.enablePeaking;
        scCfgPrms->scAdvCfg->bypass = scHalCfg.bypass;
        scCfgPrms->scAdvCfg->rowAccInc = scHalCfg.rowAccInc;
        scCfgPrms->scAdvCfg->rowAccOffset = scHalCfg.rowAccOffset;
        scCfgPrms->scAdvCfg->rowAccOffsetB = scHalCfg.rowAccOffsetB;
        scCfgPrms->scAdvCfg->ravScFactor = scHalCfg.ravScFactor;
        scCfgPrms->scAdvCfg->ravRowAccInit = scHalCfg.ravRowAccInit;
        scCfgPrms->scAdvCfg->ravRowAccInitB = scHalCfg.ravRowAccInitB;
        if (NULL != scCfgPrms->scAdvCfg->peakingCfg)
        {
            VpsUtils_memcpy(
                scCfgPrms->scAdvCfg->peakingCfg,
                scHalCfg.peakingCfg,
                sizeof(Vps_ScPeakingConfig));
        }
        if (NULL != scCfgPrms->scAdvCfg->edgeDetectCfg)
        {
            VpsUtils_memcpy(
                scCfgPrms->scAdvCfg->edgeDetectCfg,
                scHalCfg.edgeDetectCfg,
                sizeof(Vps_ScEdgeDetectConfig));
        }
    }

    return (retVal);
}



static Int32 vcoreVipAllocPath(Vcore_VipHandleObj *hObj,
                               Vcore_VipChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  streamCnt;
    Vcore_VipInstObj       *instObj;
    Vcore_VipStreamObj     *streamObj;

    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != chObj));
    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));

    /* For each of this streams, set the VIP output data format */
    for (streamCnt = 0u; streamCnt < hObj->resAllocParams.numOutParams;
            streamCnt ++)
    {
        /* Get the Stream Object from the channel object */
        streamObj = &(chObj->streamObj[streamCnt]);
        switch (streamObj->corePrms.fmt.dataFormat)
        {
            case FVID2_DF_YUV422I_YUYV:
            case FVID2_DF_YUV422I_UYVY:
              hObj->resAllocParams.outParams[streamCnt].format
                = VCORE_VIP_OUT_FORMAT_YUV422;
              break;

            case FVID2_DF_YUV420SP_UV:
            case FVID2_DF_YUV420SP_VU:
              hObj->resAllocParams.outParams[streamCnt].format
                = VCORE_VIP_OUT_FORMAT_YUV420;
              break;

            case FVID2_DF_RGB24_888:
              hObj->resAllocParams.outParams[streamCnt].format
                = VCORE_VIP_OUT_FORMAT_RGB888;
              break;

            case FVID2_DF_YUV444I:
              hObj->resAllocParams.outParams[streamCnt].format
                = VCORE_VIP_OUT_FORMAT_YUV444;
              break;

            case FVID2_DF_RAW_VBI:
              hObj->resAllocParams.outParams[streamCnt].format
                = VCORE_VIP_OUT_FORMAT_ANC;
              break;

#ifdef VPS_CAPT_422SP_SUPPORT
            case FVID2_DF_YUV422SP_UV:
                hObj->resAllocParams.outParams[streamCnt].format
                    = VCORE_VIP_OUT_FORMAT_YUV422SP_UV;
#endif

            default:
              retVal = FVID2_EFAIL;
              break;
        }
        /* If scalar is enabled in multiple output streams, make sure
           input/outptu size is same in all the streams. Core does not
           check for this error condition. */
        hObj->resAllocParams.outParams[streamCnt].scEnable =
            streamObj->corePrms.isScReq;
    }
    retVal = Vcore_vipResAllocPath(
                &hObj->resObj,
                &hObj->resAllocParams);

    return (retVal);
}



static Int32 vcoreVipFreePath(Vcore_VipHandleObj *hObj,
                              Vcore_VipChObj *chObj)
{
    Int32                   retVal = VPS_SOK;

    GT_assert(VipCoreTrace, (NULL != hObj));
    GT_assert(VipCoreTrace, (NULL != chObj));

    retVal = Vcore_vipResFreePath(&(hObj->resObj));

    return (retVal);
}



static Int32 vcoreVipSetConfig(Vcore_VipHandleObj *hObj,
                               UInt32 chNum)
{
    Int32               retVal = VPS_SOK;
    UInt32              ovlyOffset;
    UInt32              halCnt;
    Vcore_VipInstObj   *instObj;
    Vcore_VipChObj     *chObj;
    Vcore_VipStreamObj *streamObj;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(VipCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    instObj = hObj->parent;
    GT_assert(VipCoreTrace, (NULL != instObj));
    chObj = vcoreVipGetChObj(hObj, chNum);
    GT_assert(VipCoreTrace, (NULL != chObj));
    streamObj = &(chObj->streamObj[VCORE_VIP_STREAMID]);

    /* Steps
     * 1, Get the channel object
     * 2, Figure out the state for each of the channel
     * 3, Set the scalar config
     * 4, Set the CSC config
     * 5, Update DescInfo structure of this handleObj
     */

    if (VPS_SOK == retVal)
    {
        /* Figure out the the required modules
           depending on parameter set */
        retVal = vcoreVipFigureOutState(
                    hObj,
                    streamObj);
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE == streamObj->isModReq[VCORE_VIP_SC_IDX])
        {
            /* Set the configuration for each of the HALs */
            retVal = vcoreVipSetScHalCfg(streamObj);
        }
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE == streamObj->isModReq[VCORE_VIP_CSC_IDX])
        {
            VpsUtils_memcpy(
                &(streamObj->cscHalCfg),
                &(hObj->resObj.cscConfig),
                sizeof(Vps_CscConfig));
        }
    }

    if (VPS_SOK == retVal)
    {
        /*
         *  Set the channel information depending on VIPs requirement.
         *  Only one out descriptor is required.
         */
        streamObj->descInfo.numOutDataDesc = streamObj->numDesc;
        streamObj->descInfo.shadowOvlySize = 0u;
        ovlyOffset = 0u;
        for (halCnt = 0u; halCnt < VCORE_VIP_MAX_HAL; halCnt++)
        {
            if (TRUE == streamObj->isModReq[halCnt])
            {
                streamObj->ovlyOffset[halCnt] = ovlyOffset;
                streamObj->descInfo.shadowOvlySize +=
                    instObj->halShwOvlySize[halCnt];
                ovlyOffset += instObj->halShwOvlySize[halCnt];
            }
        }

        /* Non-Shadow Overlay size will be size of the overlay
           used for configuring vip muxes */
        streamObj->descInfo.nonShadowOvlySize =
            Vcore_vipGetConfigOvlySize(&(hObj->resObj));

        /* Get the horizontal, vertical and bilinear (if applicable) coeff
         * overlay sizes if scalar is needed.
         */
        if (TRUE == streamObj->isModReq[VCORE_VIP_SC_IDX])
        {
            VpsHal_scGetCoeffOvlySize(
                instObj->halHandle[VCORE_VIP_SC_IDX],
                &(streamObj->descInfo.horzCoeffOvlySize),
                &(streamObj->descInfo.vertCoeffOvlySize),
                &(streamObj->descInfo.vertBilinearCoeffOvlySize));

            streamObj->descInfo.coeffConfigDest =
                VpsHal_scGetVpdmaConfigDest(
                    instObj->halHandle[VCORE_VIP_SC_IDX]);
        }
        else
        {
            streamObj->descInfo.horzCoeffOvlySize = 0u;
            streamObj->descInfo.vertCoeffOvlySize = 0u;
            streamObj->descInfo.vertBilinearCoeffOvlySize = 0u;
        }
        chObj->state.isParamsSet = TRUE;
    }

    return (retVal);
}
