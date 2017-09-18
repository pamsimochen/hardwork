/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_CscCigPath.c
 *
 *  \brief VPS CSC+CIG path core file.
 *  This file implements the core layer for the CSC+CIG paths
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
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/vpshal_comp.h>
#include <ti/psp/vps/hal/vpshal_cig.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_cscCig.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief
 * Maximum number of handles supported per CSC+CIG path instance.
 */
#define VCORE_CSC_CIG_MAX_HANDLES           (4u)

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_CSC_CIG_MAX_HANDLES *
 *  VCORE_CSC_CIG_MAX_CHANNELS_PER_INST objects per instance,
 *  only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_CSC_CIG_MAX_CHANNEL_OBJECTS    (VCORE_CSC_CIG_MAX_CHANNELS_PER_INST * \
                                          VCORE_CSC_CIG_NUM_INST)

/** \brief Time out to be used in sem pend */
#define VCORE_CSC_CIG_SEM_TIMEOUT            (BIOS_WAIT_FOREVER)

/** \brief Default width used for initializing format structure.*/
#define VCORE_CSC_CIG_DEFAULT_WIDTH          (720u)

/** \brief Default height used for initializing format structure. */
#define VCORE_CSC_CIG_DEFAULT_HEIGHT         (480u)

/** \brief Maximum number of Data descriptor required by this core
 *  under worst case.
 */
#define VCORE_CSC_CIG_MAX_DATA_DESC      (0u)

/** \brief Maximum VPDMA channels to be programmed  */
#define VCORE_CSC_CIG_MAX_VPDMA_CHANNELS (0u)

/**
 *  \brief Number of descriptors needed per Buffer.
 */
#define VCORE_CSC_CIG_NUM_DESC_PER_WINDOW    (0u)

/** \brief Number of VPDMA frame start event regsiter. */
#define VCORE_CSC_CIG_NUM_VPDMA_FS_EVT_REG  (0u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief
 * Typedef for CSC+CIG path instance object.
 */
typedef struct Vcore_CscCigInstObj_t Vcore_CscCigInstObj;

/**
 *  struct Vcore_CscCigChObj
 *  \brief Structure containing per channel information.
 */
typedef struct
{
    Vcore_States            state;
    /**< Current state */
    Vcore_Format            coreFmt;
    /**< Buffer formats and other formats */
    Vps_CscConfig           cscConfig;
    /**< Color Space converter configuration.  This is to pass to the CSC HAL
         It will be configured once during handle open.
         After that only relevant info will be configure for each queue. */
    UInt32                  cscConfigDone;
    /**< Flag to indicate whether CSC configuration params are done */
    VpsHal_CigConfig        cigConfig;
    /**< Constrained Image generator configuration.  This is to pass to the CIG HAL
         It will be configured once during handle open.
         After that only relevant info will be configure for each queue. */
    UInt32                  cigConfigDone;
    /**< Flag to indicate whether CIG configuration params are done */
    VpsHal_CompConfig       compConfig;
    /**< Compositor/Blender configuration.  This is to pass to the COMP HAL
         It will be configured once during handle open.
         After that only relevant info will be configure for each queue. */
    UInt32                  compConfigDone;
    /**< Flag to indicate whether COMP configuration params are done */
    Vps_CscConfig       cscWrbkConfig;
    /**< Color Space converter configuration.  This is to pass to the CSC HAL
         It will be configured once during handle open.
         After that only relevant info will be configure for each queue. */
    UInt32                  cscWrbkConfigDone;
    /**< Flag to indicate whether CSC configuration params are done */
    UInt32                  isAllocated;
    /**< Flag to  indicate channel object is allocated or not */
} Vcore_CscCigChObj;

/**
 *  struct Vcore_CscCigHandleObj
 *  \brief Structure containing per handle information.
 */
typedef struct
{
    UInt32                  used;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_CscCigInstObj        *parent;
    /**< Pointer to the instance object. */
    UInt32                  perChCfg;
    /**< Flag indicating whether format is common for all the channel or
         it is different for each channel. */
    Vcore_CscCigChObj          handleContext;
    /**< Used when the handle is opened with perChCfg as false.
         Even in this mode, the data descriptor information is not updated
         in this variable - still the channel objects is used for this
         purpose. */
    UInt32                  numCh;
    /**< Number of channel for this handle. This determines the number of
         valid pointers in above channel array. */
    Vcore_CscCigChObj          *chObjs[VCORE_CSC_CIG_MAX_CHANNELS_PER_INST];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool */
} Vcore_CscCigHandleObj;

/**
 *  struct Vcore_CscCigInstObj_t
 *  \brief Structure containing per instance information.
 */
struct Vcore_CscCigInstObj_t
{
    UInt32                  instId;
    /**< Instance number/ID. */
    UInt32                  openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    Vcore_OpMode            curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or mem-mem operation of the core. */
    Vcore_CscCigHandleObj      *hObjs;
    /**< Pointer to the handle information. This is a contiguous memory of
         numHandles Vcore_CscCigHandleObj structure */
    UInt32                  numHandle;
    /**< Number of handles for this instance. This determines the size of
         handle array. */
    VpsHal_Handle           cscHandle;
    /**< Handle of CSC HAL.  This will be used to configure CSC. */
    VpsHal_Handle           cigHandle;
    /**< Handle of CIG HAL.  This will be used to configure CIG. */
    VpsHal_Handle           blendHdmiHandle;
    /**< Handle of HDMI Blender HAL.  This will be used to configure Blender. */
    VpsHal_Handle           cscWrbkHandle;
    /**< Handle of CSC HAL.  This will be used to configure Writeback CSC. */
//    VpsHal_VpsClkcModule    module;
//    /**< Module enum for initializing the clock */
};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


static Vcore_Handle Vcore_cscCigOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg);
static Int32 Vcore_cscCigClose(Vcore_Handle handle);
static Int32 Vcore_cscCigSetFormat(Vcore_Handle handle,
                                UInt32 channel,
                                const Vcore_Format *coreFmt);
static Int32 Vcore_cscCigGetProperty(Vcore_Handle handle,
                                 Vcore_Property *property);
static Int32 Vcore_cscCigGetFormat(Vcore_Handle handle,
                                UInt32 channel,
                                Vcore_Format *coreFmt);
static Int32 Vcore_cscCigGetDescInfo(Vcore_Handle handle,
                                  UInt32 channel,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo);
static Int32 Vcore_cscCigSetFSEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_cscCigProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem);
static Int32 Vcore_cscCigProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_cscCigUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtParams,
                                 UInt32 frmIdx);
static Int32 Vcore_cscCigUpdateDesc(Vcore_Handle handle,
                                const Vcore_DescMem *descMem,
                                const FVID2_Frame *frame,
                                UInt32 fid);

static Int32 cscCigCheckFormat(const Vcore_CscCigHandleObj *hObj,
                            const Vcore_Format *coreFmt);
static Vcore_CscCigHandleObj *cscCigAllocHandleObj(UInt32 instId,
                                             Vcore_OpMode mode);

static Int32 cscCigFreeHandleObj(Vcore_CscCigHandleObj *hObj);

static Int32 cscCigAllocChannelObj(Vcore_CscCigChObj **chObjs,
                                UInt32 numCh);

static Int32 cscCigFreeChannelObj(Vcore_CscCigChObj **chObjs,
                               UInt32 numCh);

static void cscCigSetDefaultChInfo(Vcore_CscCigChObj *chObj,
                                 UInt32 channelCnt);

static Vcore_CscCigHandleObj *cscCigAllocHandleMem(UInt32 numHandle);

static Int32 cscCigFreeHandleMem(const Vcore_CscCigHandleObj *hObjs);

static Vcore_CscCigChObj *cscCigAllocChannelMem(void);

static Int32 cscCigFreeChannelMem(const Vcore_CscCigChObj *chObj);

static UInt32 cscCigProgramReg(Vcore_CscCigHandleObj *hObj,
                               Vcore_CscCigChObj *chObj,
                              const Vcore_DescMem *descMem);
static Int32 cscCigUpdateRtParams(const Vcore_CscCigHandleObj *hObj,
                               Vcore_CscCigChObj *chObj,
                               const Vcore_CscCigRtParams *rtParams);
static Vcore_CscCigChObj *cscCigGetChObj(Vcore_CscCigHandleObj *hObj, UInt32 chNum);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief CSC+CIG Path Core function pointer. */
static const Vcore_Ops CscCigCoreOps =
{
    Vcore_cscCigGetProperty,               /* Get property */
    Vcore_cscCigOpen,                      /* Open function */
    Vcore_cscCigClose,                     /* Close function */
    Vcore_cscCigSetFormat,                 /* Set format */
    Vcore_cscCigGetFormat,                 /* Get format */
    NULL,                               /* Set mosaic */
    NULL,                               /* Create layout */
    NULL,                               /* Delete layout */
    Vcore_cscCigGetDescInfo,            /* Get descriptor */
    Vcore_cscCigSetFSEvent,             /* Frame start event for Handle */
    NULL,                               /* Set params */
    NULL,                               /* Get params */
    NULL,                               /* Control */
    Vcore_cscCigProgramDesc,            /* Program Descriptors */
    Vcore_cscCigProgramReg,             /* Program registors/overlay memory */
    Vcore_cscCigUpdateDesc,             /* Update the descriptors */
    NULL,                               /* Update Multidesc */
    NULL,                               /* Update context */
    NULL,                               /* Get DEI ctx info funct pointer. */
    Vcore_cscCigUpdateRtMem,            /* Update run time configuration */
    NULL                                /* Program scalar coefficient */
};
/** \brief CSC+CIG path objects */
static Vcore_CscCigInstObj CscCigInstObjects[VCORE_CSC_CIG_NUM_INST];

/** \brief Properties of this core */
static Vcore_Property CscCigProperty;

/**
 *  \brief Memory pool for the Handle objects per instance.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_CscCigHandleObj CscCigHandleMemPool
            [VCORE_CSC_CIG_NUM_INST][VCORE_CSC_CIG_MAX_HANDLES];

/**
 *  \brief The flag variable represents whether a given handle object is
 *  allocated or not
 */
static UInt32 CscCigHandleMemFlag[VCORE_CSC_CIG_NUM_INST];

/**
 *  \brief Memory pool for the channel objects.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_CscCigChObj CscCigChannelMemPool[VCORE_CSC_CIG_MAX_CHANNEL_OBJECTS];

/** \brief Semaphore for protecting allocation and freeing of memory pool
    objects */
static Semaphore_Handle CscCigPoolSem = NULL;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_cscCigInit
 *  \brief CSC+CIG Path Core init function.
 *  Initializes CSC+CIG Path core objects, allocates memory etc.
 *  This function should be called before calling any of CSC_CIG core API's.
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
Int32 Vcore_cscCigInit(UInt32 numInstance,
                   const Vcore_CscCigInitParams *initParams,
                   Ptr arg)
{
    UInt32              instCnt, handleCnt;
    Vcore_CscCigInstObj    *instObj;
    Int32               retVal = VPS_SOK;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(CscCigCoreTrace, (numInstance <= VCORE_CSC_CIG_NUM_INST));
    GT_assert(CscCigCoreTrace, (NULL != initParams));

    /* Initialize cscCig Objects to zero */
    VpsUtils_memset(CscCigInstObjects, 0, sizeof(CscCigInstObjects));
    VpsUtils_memset(&CscCigProperty, 0, sizeof(CscCigProperty));
    VpsUtils_memset(&CscCigHandleMemPool, 0, sizeof(CscCigHandleMemPool));
    VpsUtils_memset(&CscCigChannelMemPool, 0, sizeof(CscCigChannelMemPool));

    /* Mark pool flags as free */
    for (instCnt = 0u; instCnt < VCORE_CSC_CIG_NUM_INST; instCnt++)
    {
        CscCigHandleMemFlag[instCnt] = FALSE;
        for (handleCnt = 0u; handleCnt < VCORE_CSC_CIG_MAX_HANDLES; handleCnt++)
        {
            CscCigHandleMemPool[instCnt][handleCnt].used = FALSE;
        }
    }

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    CscCigPoolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == CscCigPoolSem)
    {
        GT_0trace(CscCigCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        /* Initialize CSC+CIG  path core properties */
        CscCigProperty.numInstance = numInstance;
        CscCigProperty.name = VCORE_TYPE_CSC_CIG;
        CscCigProperty.type = VCORE_TYPE_INTERMEDIATE;
        CscCigProperty.internalContext = FALSE;
        for (instCnt = 0u; instCnt < numInstance; instCnt++)
        {
            /* Initialize instance object members */
            instObj = &CscCigInstObjects[instCnt];
            instObj->instId = initParams[instCnt].instId;
            instObj->cscHandle = initParams[instCnt].cscHandle;
            instObj->cigHandle = initParams[instCnt].cigHandle;
            instObj->blendHdmiHandle = initParams[instCnt].blendHdmiHandle;
            instObj->cscWrbkHandle = initParams[instCnt].cscWrbkHandle;
            instObj->curMode = VCORE_OPMODE_INVALID;
            /* Allocate handle object memory */
            instObj->hObjs = cscCigAllocHandleMem(
                                  initParams[instCnt].maxHandle);
            if (NULL == instObj->hObjs)
            {
                GT_1trace(CscCigCoreTrace, GT_ERR,
                    "Handle allocation failed for %d!!\n", instCnt);
                retVal = FVID2_EALLOC;
                break;
            }
            instObj->numHandle = initParams[instCnt].maxHandle;
        }
    }
    /* Cleanup if error occurs */
    if (VPS_SOK != retVal)
    {
        Vcore_cscCigDeInit(NULL);
    }
    return (retVal);
}

/**
 *  Vcore_cscCigDeInit
 *  \brief CSC+CIG Path exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_cscCigDeInit(Ptr arg)
{
    UInt32              instCnt;
    Vcore_CscCigInstObj    *instObj;

    /* Free-up handle objects for each instance */
    for (instCnt = 0u; instCnt < CscCigProperty.numInstance; instCnt++)
    {
        instObj = &CscCigInstObjects[instCnt];
        instObj->instId = VCORE_CSC_CIG_INST_0;
        instObj->curMode = VCORE_OPMODE_INVALID;
        if (instObj->hObjs)
        {
            cscCigFreeHandleMem(instObj->hObjs);
            instObj->hObjs = NULL;
            instObj->numHandle = 0u;
        }
    }
    CscCigProperty.numInstance = 0u;

    /* Delete the Semaphore created for Pool objects */
    if (CscCigPoolSem)
    {
        Semaphore_delete(&CscCigPoolSem);
        CscCigPoolSem = NULL;
    }

    return (VPS_SOK);
}

/**
 *  Vcore_cscCigGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_cscCigGetCoreOps(void)
{
    return &CscCigCoreOps;

}

/**
 *  Vcore_cscCigOpen
 *  \brief Opens a particular instance of CSC+CIG in either
 *  display or mem-mem mode and returns handle.
 *  When opened in display mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in mem-mem mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_CSC_CIG_MAX_HANDLES macro. Also each of the handle could have multiple
 *  channel to support in a single handle.  The maximum number of channels for
 *  all the handles is determined by
 *
 *  \param instId       Instance to open - Indpendent CSC+CIG path 0
 *                      or  CSC+CIG Path 1.
 *  \param mode         Mode to open the instance for - display or mem-mem.
 *  \param numCh   Number of channel to be associated with this open.
 *                      For display operation only one channel is allowed.
 *                      For mem-mem operation, more than one channel could
 *                      be associated with a handle handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_CSC_CIG_MAX_CHANNELS_PER_INST macro.
 *  \param perChCfg Flag indicating whether the config descriptors
 *                          should be separate for each and every channel or
 *                          not. This is valid only for mem-mem drivers. For
 *                          display drivers this should be set to FALSE.
 *                          TRUE - Separate config descriptors for each channel.
 *                          FALSE - One config descriptors used per handle.
 *
 *  \return             If success returns the handle else returns NULL.
 */
static Vcore_Handle Vcore_cscCigOpen(UInt32 instId,
                          Vcore_OpMode mode,
                          UInt32 numCh,
                          UInt32 perChCfg)
{
    Vcore_CscCigHandleObj      *hObj = NULL;
    Int32               retVal = VPS_SOK;
    Vcore_CscCigInstObj    *instObj = NULL;
    UInt32              instCnt;

    /* Only memory mode supported for Instance 0*/
    if (instId == VCORE_CSC_CIG_INST_0)
    {
        GT_assert(CscCigCoreTrace, (VCORE_OPMODE_MEMORY == mode));
    }

    /* Check for maximum channel supported per handle */
    GT_assert(CscCigCoreTrace, (numCh <= VCORE_CSC_CIG_MAX_CHANNELS_PER_INST));
    /* There should be atleast one channel */
    GT_assert(CscCigCoreTrace, (0u != numCh));

    /* For display operation, only one channel should be allowed */
    GT_assert(CscCigCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (VCORE_MAX_DISPLAY_CH < numCh))));

    /* For display operation, per channel format should be FALSE */
    GT_assert(CscCigCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (FALSE != perChCfg))));

    for (instCnt = 0u; instCnt < CscCigProperty.numInstance; instCnt++)
    {
        if (CscCigInstObjects[instCnt].instId == instId)
        {
            instObj = &CscCigInstObjects[instCnt];
            break;
        }
    }
    if (VPS_SOK == retVal && (NULL != instObj))
    {
        /* Allocate a handle object */
        hObj = cscCigAllocHandleObj(instId, mode);
        if (NULL != hObj)
        {
            /* Allocate channel objects */
            retVal = cscCigAllocChannelObj(hObj->chObjs, numCh);
            if (VPS_SOK != retVal)
            {
                GT_0trace(CscCigCoreTrace, GT_ERR, "Channel allocation failed!!\n");
                /* Free-up handle object if channel allocation failed */
                cscCigFreeHandleObj(hObj);
                hObj = NULL;
            }
            else
            {
                /* Initialize variables */
                hObj->perChCfg = perChCfg;
                hObj->numCh = numCh;
                cscCigSetDefaultChInfo(&hObj->handleContext, 0u);
            }
        }
        else
        {
            GT_0trace(CscCigCoreTrace, GT_ERR, "Handle allocation failed!!\n");
        }
    }

    if (NULL != hObj && NULL != instObj)
    {
        if (0 == instObj->openCnt && NULL != instObj)
        {
//?TODO: Check for any clk en needed            retVal = VpsHal_vpsClkcModuleEnable(instObj->module, TRUE);

            if (VPS_SOK != retVal)
            {
                cscCigFreeChannelObj(hObj->chObjs, hObj->numCh);
                cscCigFreeHandleObj(hObj);
                hObj = NULL;
            }
        }

    }
    if (NULL != hObj && NULL != instObj)
    {
        instObj->openCnt++;
    }
    return ((Vcore_Handle) hObj);
}

/**
 *  Vcore_cscCigClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       CSC+CIG path handle.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_cscCigClose(Vcore_Handle handle)
{
    Int32                   retVal = VPS_EFAIL;
    Vcore_CscCigHandleObj      *hObj;
    Vcore_CscCigInstObj        *instObj = NULL;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));

    hObj = (Vcore_CscCigHandleObj *) handle;

    instObj = hObj->parent;
    GT_assert(CscCigCoreTrace, (NULL != instObj));
    /* Free channel objects */
    retVal = cscCigFreeChannelObj(hObj->chObjs, hObj->numCh);
    /* Free handle objects */
    retVal += cscCigFreeHandleObj(hObj);
    /* Null the handle for sake of completeness */
    hObj = NULL;
    Semaphore_pend(CscCigPoolSem, VCORE_CSC_CIG_SEM_TIMEOUT);
    /* Decrement the count by one */
    instObj->openCnt--;
    if (0 == instObj->openCnt)
    {
//?        retVal += VpsHal_vpsClkcModuleEnable(instObj->module, FALSE);
    }
    Semaphore_post(CscCigPoolSem);
    return (retVal);
}

/**
 *  Vcore_CscCigGetProperty
 *  \brief Gets the core properties of the CSC+CIG path core.
 *
 *  \param handle       CSC+CIG Path Core handle.
 *  \param property     Pointer to which the CSC+CIG path core properties to
 *                      be copied.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_cscCigGetProperty(Vcore_Handle handle,
                                 Vcore_Property *property)
{
    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));
    GT_assert(CscCigCoreTrace, (NULL != property));

    VpsUtils_memcpy(property, &CscCigProperty, sizeof(Vcore_Property));

    return (VPS_SOK);
}

/**
 *  Vcore_cscCigSetFormat.
 *  \brief sets the format for the core. Initializes how many data and config
 *  descriptors will be required depending upon the format passed.
 *
 *  \param handle       CSC+CIG Path handle.
 *  \param channel      Channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param coreFmt      Pointer to core foramt information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_cscCigSetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_Format *coreFmt)
{
    Int32                   retVal = VPS_SOK;
    Vcore_CscCigHandleObj      *hObj;
    Vcore_CscCigChObj          *chObj;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));
    GT_assert(CscCigCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_CscCigHandleObj *) handle;

     /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    /* Check if the channel is within the range allocated during open */
    if (VPS_SOK == retVal)
    {
        chObj = cscCigGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(CscCigCoreTrace, (NULL != chObj));

        /* Check whether the requested format is valid */
        retVal = cscCigCheckFormat(hObj, coreFmt);
        if (VPS_SOK != retVal)
        {
            GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid Format!!\n");
        }
    }
    if (VPS_SOK == retVal)
    {
#if 0
        if (VCORE_OPMODE_MEMORY == hObj->parent->curMode)
        {
            /* Get the overlay memory size of the chroma up sampler */
            chObj->descInfo.shadowOvlySize =
                VpsHal_cscGetConfigOvlySize(hObj->parent->cscHandle) +
                VpsHal_cscGetConfigOvlySize(hObj->parent->cscWrbkHandle) +
                VpsHal_cigGetConfigOvlySize(hObj->parent->cigHandle,
                                            VPSHAL_COT_PIP_ENABLE_CONFIG) +
                VpsHal_compGetConfigOvlySize(hObj->parent->blendHdmiHandle,
                                            VPSHAL_COMP_SELECT_HDMI);
        }
        else
        {
            chObj->descInfo.shadowOvlySize = 0u;
        }

        if (VCORE_OPMODE_MEMORY == hObj->parent->curMode)
        {
            /* Get the overlay memory size required and
             * virtual register address for that client register to
             * program it into overlay memory */
                chObj->descInfo.nonShadowOvlySize = 0;
        }
        else
        {
            chObj->descInfo.nonShadowOvlySize = 0;
        }
#endif
        /* Copy the format to the local structure */
        VpsUtils_memcpy(&chObj->coreFmt,
            (Ptr) coreFmt,
            sizeof(Vcore_Format));
        /* Set proper states - since format is set, desc info should be set
         * again */
        chObj->state.isFormatSet = TRUE;

    }
    return (retVal);
}

/**
 *  Vcore_cscCigGetFormat
 *  \brief Gets the format for a given channel.
 *
 *  \param handle       CSC+CIG Path handle.
 *  \param chNum      Channel from which the format should be read.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param coreFmt      Pointer to the copied core format information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_cscCigGetFormat(Vcore_Handle handle,
                        UInt32 chNum,
                        Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
    Vcore_CscCigHandleObj      *hObj;
    Vcore_CscCigChObj *chObj;

     /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));
    GT_assert(CscCigCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_CscCigHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = cscCigGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(CscCigCoreTrace, (NULL != chObj));

        /* Copy the format */
        VpsUtils_memcpy((Ptr)coreFmt, &chObj->coreFmt, sizeof(Vcore_Format));
    }

    return (retVal);
}

/**
 *  Vcore_cscCigGetDescInfo
 *  \brief Depending on the format set, returns the number of data
 *  descriptors needed for the actual driver to allocate memory.
 *  Format should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       CscCig Path handle.
 *  \param channel      Channel to which the info is required.
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
static Int32 Vcore_cscCigGetDescInfo(Vcore_Handle handle,
                              UInt32 chNum,
                              UInt32 layoutId,
                              Vcore_DescInfo *descInfo)
{
    Int32               retVal = VPS_SOK;
    Vcore_CscCigHandleObj      *hObj;
    Vcore_CscCigChObj *chObj;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));
    GT_assert(CscCigCoreTrace, (NULL != descInfo));

    hObj = (Vcore_CscCigHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = cscCigGetChObj(hObj, chNum);

        if (TRUE == chObj->state.isFormatSet)
        {
            retVal = VPS_SOK;
            /* Copy the data descriptor information */
            descInfo->numInDataDesc        = 0;
            descInfo->numMultiWinDataDesc  = 0;
            descInfo->numOutDataDesc       = 0;
            descInfo->shadowOvlySize       = 0;
            descInfo->nonShadowOvlySize    = 0;
            descInfo->horzCoeffOvlySize    = 0;
            descInfo->vertCoeffOvlySize    = 0;
            descInfo->vertBilinearCoeffOvlySize = 0;
            descInfo->numChannels          = 0;
        }
        else
        {
             retVal = VPS_EFAIL;
            GT_0trace(CscCigCoreTrace, GT_ERR,
                "Set the parameter before getting the descriptor info!!\n");
        }
    }
    return (retVal);
}

/**
 *  \brief Program the client's frame start.
 *
 *  \param handle       Secondary Path handle.
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
static Int32 Vcore_cscCigSetFSEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem)
{
    Int32               retVal = VPS_SOK;
    /* no FS event for this core to program*/
    return (retVal);
}

/**
 *  Vcore_cscCigProgramDesc
 *  \brief Programs the data descriptor for a given channel. This will not
 *  program the internal context descriptors.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       CSC+CIG Path Core handle.
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
static Int32 Vcore_cscCigProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    /* no descriptors for this core to program*/
    return (retVal);
}

/**
 *  Vcore_cscCigProgramReg
 *  \brief Programs the registers for HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       CSC+CIG Path Core handle.
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
static Int32 Vcore_cscCigProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_CscCigHandleObj     *hObj;
    Vcore_CscCigChObj    *chObj;
    Vcore_CscCigInstObj       *instObj;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));
    hObj = (Vcore_CscCigHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(CscCigCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = cscCigGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(CscCigCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(CscCigCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }
    if (VPS_SOK == retVal)
    {
        retVal = cscCigProgramReg(hObj, chObj, descMem);
    }
    return (retVal);
}


/**
 *  Vcore_cscCigUpdateRtMem
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       CSC+CIG Path Core handle.
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
 *  \param rtParams     Per frame runtime configuration structure pointer.
 *  \param frmIdx       Not used currently. Meant for future purpose.
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_cscCigUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtParams,
                                 UInt32 frmIdx)
{
    Int32                   retVal = VPS_SOK;
    Vcore_CscCigHandleObj      *hObj;
    Vcore_CscCigChObj     *chObj;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != handle));
    GT_assert(CscCigCoreTrace, (NULL != descMem));

    hObj = (Vcore_CscCigHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = cscCigGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(CscCigCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(CscCigCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != rtParams)
        {
            retVal = cscCigUpdateRtParams(hObj, chObj, rtParams);
            if (VPS_SOK != retVal)
            {
                GT_1trace(CscCigCoreTrace, GT_ERR,
                    "Update RT params failed for channel %d!!\n", chNum);
            }
        }
    }

    return (retVal);
}

/**
 *  cscCigCheckParams
 *  Checks whether a given format is valid or not.
 *  Returns 0 if everything is fine else returns error value.
 */
static Int32 cscCigCheckFormat(const Vcore_CscCigHandleObj *hObj,
                            const Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
    Vcore_CscCigInstObj   *instObj;

    GT_assert(CscCigCoreTrace, (NULL != hObj));
    GT_assert(CscCigCoreTrace, (NULL != coreFmt));
    instObj = hObj->parent;
    GT_assert(CscCigCoreTrace, (NULL != instObj));

    if (FVID2_DF_YUV420SP_UV == coreFmt->fmt.dataFormat ||
            FVID2_DF_YUV422SP_UV == coreFmt->fmt.dataFormat)
    {
        if (((coreFmt->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] <
                (coreFmt->fmt.width))) ||
        (coreFmt->fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] <
        (coreFmt->fmt.width))) {
            GT_2trace(CscCigCoreTrace, GT_ERR,
            "Pitch (%d) less than Width (%d) in bytes!!\n",
            coreFmt->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX],
            (coreFmt->fmt.width));
            retVal = VPS_EINVALID_PARAMS;
        }
    }
    else if (FVID2_DF_YUV422I_YUYV == coreFmt->fmt.dataFormat)
    {
        if (coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX] <
            (coreFmt->fmt.width * 2u))
        {
            GT_2trace(CscCigCoreTrace, GT_ERR,
                "Pitch (%d) less than Width (%d) in bytes!!\n",
                coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX],
                (coreFmt->fmt.width * 2u));
            retVal = VPS_EINVALID_PARAMS;
        }
    }
    if (FVID2_DF_YUV422I_YUYV != coreFmt->fmt.dataFormat &&
        FVID2_DF_YUV420SP_UV != coreFmt->fmt.dataFormat &&
        FVID2_DF_YUV422SP_UV != coreFmt->fmt.dataFormat)
    {
        GT_1trace(CscCigCoreTrace, GT_ERR,
            "Buffer format (%d) not supported!!\n",
            coreFmt->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check whether window width/startX is even */
    if ((coreFmt->fmt.width & 0x01u) || (coreFmt->startX & 0x01u))
    {
        GT_2trace(CscCigCoreTrace, GT_ERR,
            "Width(%d)/StartX(%d) can't be odd!!\n",
            coreFmt->fmt.width,
            coreFmt->startX);
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_DF_YUV420SP_UV == coreFmt->fmt.dataFormat)
    {
        if ((FVID2_SF_INTERLACED == coreFmt->fmt.scanFormat) &&
            (VCORE_OPMODE_DISPLAY == instObj->curMode))
        {
            /* Check whether window height/startY is multiple of 4 for YUV420
             * format in interlaced display mode */
            if ((coreFmt->fmt.height & 0x03u) || (coreFmt->startY & 0x03u))
            {
                GT_2trace(CscCigCoreTrace, GT_ERR,
                    "Height(%d)/StartY(%d) should be multiple of 4 for YUV420 "
                    "format in display mode!!\n",
                    coreFmt->fmt.height,
                    coreFmt->startY);
                retVal = VPS_EINVALID_PARAMS;
            }
        }
        else
        {
            /* Check whether window height/startY is even for YUV420 format */
            if ((coreFmt->fmt.height & 0x01u) || (coreFmt->startY & 0x01u))
            {
                GT_2trace(CscCigCoreTrace, GT_ERR,
                    "Height(%d)/StartY(%d) can't be odd for YUV420 format!!\n",
                    coreFmt->fmt.height,
                    coreFmt->startY);
                retVal = VPS_EINVALID_PARAMS;
            }
        }
    }

    if (coreFmt->frameWidth < (coreFmt->startX + coreFmt->fmt.width))
    {
        GT_2trace(CscCigCoreTrace, GT_ERR,
            "Frame Width (%d) less than Width + startX (%d)!!\n",
            coreFmt->frameWidth,
            (coreFmt->startX + coreFmt->fmt.width));
        retVal = VPS_EINVALID_PARAMS;
    }
    if (coreFmt->frameHeight < (coreFmt->startY + coreFmt->fmt.height))
    {
        GT_2trace(CscCigCoreTrace, GT_ERR,
            "Frame Height (%d) less than Height + startY (%d)!!\n",
            coreFmt->frameHeight,
            (coreFmt->startY + coreFmt->fmt.height));
        retVal = VPS_EINVALID_PARAMS;
    }
    /* Check if the scan format are matching */
    if ((FVID2_SF_INTERLACED == coreFmt->secScanFmt) &&
        (FVID2_SF_INTERLACED != coreFmt->fmt.scanFormat))
    {
        GT_0trace(SecCoreTrace, GT_ERR,
            "Progressive to Interlaced conversion not supported!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }
    if ((FVID2_SF_PROGRESSIVE == coreFmt->secScanFmt) &&
        (FVID2_SF_PROGRESSIVE != coreFmt->fmt.scanFormat))
    {
        GT_0trace(SecCoreTrace, GT_ERR,
            "Interlaced to Progressive conversion not supported!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }
#if 0
    if (FVID2_SOK == retVal)
    {
        retVal = VpsHal_vpdmaIsValidSize(
                    instObj->vpdmaPath,
                    coreFmt->fmt.dataFormat,
                    (VpsHal_VpdmaMemoryType) coreFmt->memType,
                    coreFmt->fmt.width,
                    coreFmt->fmt.height);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(CscCigCoreTrace, GT_ERR, "Wrong Input Size!!\n");
        }
    }
#endif
    return (retVal);
}


static UInt32 cscCigProgramReg(Vcore_CscCigHandleObj *hObj,
                               Vcore_CscCigChObj *chObj,
                              const Vcore_DescMem *descMem)
{
    Int32               retVal = VPS_EFAIL, cookie;
    Vps_CscConfig       *cscConfig;
    VpsHal_CigConfig    *cigConfig;
    VpsHal_CompConfig   *compConfig;
    Vps_CscConfig       *cscWrbkConfig;

    /* configure CSC */
    cscConfig = &chObj->cscConfig;
    cscConfig->bypass = FALSE;
    cscConfig->mode   = VPS_CSC_MODE_HDTV_GRAPHICS_Y2R;
    cscConfig->coeff  = NULL;

    retVal = VpsHal_cscSetConfig(hObj->parent->cscHandle,
                                 cscConfig, NULL);
    /* configure CIG PIP path */
    if (VPS_SOK == retVal)
    {
        chObj->cscConfigDone = TRUE;

        cigConfig = &chObj->cigConfig;
        /* Since configuration for Main path and PIP path is in a
           single register and main path and PIP could be used in display as
           well as mem2mem driver, Read and write to CIG register
           should be atomic. */
        cookie = Hwi_disable();

        /* Get the Default PIP Configuration */
        retVal = VpsHal_cigGetConfig(hObj->parent->cigHandle, cigConfig);
        if (0 == retVal)
        {
            /* Configure PIP Parameters */
            cigConfig->pipConfig.xPos = 0u;
            cigConfig->pipConfig.yPos = 0u;
            cigConfig->pipConfig.dispWidth  = chObj->coreFmt.frameWidth; /*?*/
            cigConfig->pipConfig.dispHeight = chObj->coreFmt.frameHeight; /*?*/
            cigConfig->pipConfig.pipWidth   = chObj->coreFmt.frameWidth;
            cigConfig->pipConfig.pipHeight  = chObj->coreFmt.frameHeight;

            /* Set the CIG PIP parameters using Hal layer */
            retVal = VpsHal_cigSetConfig(hObj->parent->cigHandle, cigConfig, NULL);
        }
        Hwi_restore(cookie);
    }

    /* configure Blender */
    if (VPS_SOK == retVal)
    {
        chObj->cigConfigDone = TRUE;

        compConfig = &chObj->compConfig;
        /* Get the Comp Config from the COMP */
        retVal = VpsHal_compGetConfig(
                    hObj->parent->blendHdmiHandle,
                    compConfig,
                    VPSHAL_COMP_SELECT_HDMI);
        compConfig->fbPath = VPSHAL_COMP_OUT_FINAL_ALPHA_BLENDING;
        compConfig->videoInConfig.hdmiConfig.hdmiBase =
                                VPSHAL_COMP_HDMI_BLEND_BASE_HD_PIP;
        //compConfig->gReorderMode = VPSHAL_COMP_GLOBAL_REORDER_ON; /*?*/

        if (VPS_SOK == retVal)
        {
            /* Set the configuration in the Comp */
            retVal = VpsHal_compSetConfig(
                        hObj->parent->blendHdmiHandle,
                        compConfig,
                        VPSHAL_COMP_SELECT_HDMI,
                        NULL);
        }
    }

    /* configure writeback path CSC before Sc5 */
    if (VPS_SOK == retVal)
    {
        chObj->compConfigDone = TRUE;

        cscWrbkConfig = &chObj->cscWrbkConfig;
        cscWrbkConfig->bypass = FALSE;
        cscWrbkConfig->mode   = VPS_CSC_MODE_HDTV_GRAPHICS_R2Y;
        cscWrbkConfig->coeff  = NULL;
        retVal = VpsHal_cscSetConfig(hObj->parent->cscWrbkHandle,
                                     cscWrbkConfig, NULL);
        if (VPS_SOK == retVal)
        {
            chObj->cscWrbkConfigDone = TRUE;
        }
    }

    return (retVal);
}

static Int32 cscCigUpdateRtParams(const Vcore_CscCigHandleObj *hObj,
                               Vcore_CscCigChObj *chObj,
                               const Vcore_CscCigRtParams *rtParams)
{
    Int32               retVal;
    UInt32              pitchCnt;
    Vcore_Format       *coreFmt;

    /* Copy the parameter to the configuration parameters */
    coreFmt = &chObj->coreFmt;

    /* Copy the in frame params to core params */
    if (NULL != rtParams->inFrmPrms)
    {
        coreFmt->fmt.width = rtParams->inFrmPrms->width;
        coreFmt->fmt.height = rtParams->inFrmPrms->height;
        /* For M2M mode, when dimension changes, change the frame dimensions as
         * well. This should not be done for display mode as frame width and
         * frame height represents display resolution and they never change. */
        if (VCORE_OPMODE_MEMORY == hObj->parent->curMode)
        {
            coreFmt->frameWidth = rtParams->inFrmPrms->width;
            coreFmt->frameHeight = rtParams->inFrmPrms->height;
        }
        for (pitchCnt = 0u; pitchCnt < FVID2_MAX_PLANES; pitchCnt++)
        {
            coreFmt->fmt.pitch[pitchCnt] = rtParams->inFrmPrms->pitch[pitchCnt];
        }
    }

    /* Copy the position params to core params */
    if (NULL != rtParams->posCfg)
    {
        coreFmt->startX = rtParams->posCfg->startX;
        coreFmt->startY = rtParams->posCfg->startY;
    }
#if 0
    /* Dummy data descriptor is not needed if frame size and buffer
     * bottom-right coordinates match */
    if (((coreFmt->startX + coreFmt->fmt.width) < coreFmt->frameWidth) ||
        ((coreFmt->startY + coreFmt->fmt.height) < coreFmt->frameHeight))
    {
        chObj->isDummyNeeded = TRUE;
    }
    else
    {
        chObj->isDummyNeeded = FALSE;
    }
#endif
    /* Check whether the updated RT params are valid */
    retVal = cscCigCheckFormat(hObj, coreFmt);
    if (VPS_SOK != retVal)
    {
        GT_0trace(CscCigCoreTrace, GT_ERR, "Invalid runtime parameters!!\n");
    }

    return (retVal);
}


/**
 *  cscCigAllocHandleMem
 *  Allocate memory for handle objects from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_CscCigHandleObj *cscCigAllocHandleMem(UInt32 numHandle)
{
    UInt32              instCnt;
    Vcore_CscCigHandleObj *hObjs = NULL;

    /* Check if the requested number of handles is within range */
    GT_assert(CscCigCoreTrace, (numHandle <= VCORE_CSC_CIG_MAX_HANDLES));

    Semaphore_pend(CscCigPoolSem, VCORE_CSC_CIG_SEM_TIMEOUT);

    for (instCnt = 0u; instCnt < VCORE_CSC_CIG_NUM_INST; instCnt++)
    {
        if (FALSE == CscCigHandleMemFlag[instCnt])
        {
            hObjs = &CscCigHandleMemPool[instCnt][0];
            CscCigHandleMemFlag[instCnt] = TRUE;
            break;
        }
    }

    if (NULL == hObjs)
    {
        GT_0trace(CscCigCoreTrace, GT_ERR, "Handle Object memory alloc failed!!\n");
    }
    Semaphore_post(CscCigPoolSem);

    return (hObjs);
}

/**
 *  cscCigFreeHandleMem
 *  Frees-up the handle objects memory.
 */
static Int32 cscCigFreeHandleMem(const Vcore_CscCigHandleObj *hObjs)
{
    UInt32              instCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != hObjs));

    Semaphore_pend(CscCigPoolSem, VCORE_CSC_CIG_SEM_TIMEOUT);

    for (instCnt = 0; instCnt < VCORE_CSC_CIG_NUM_INST; instCnt++)
    {
        if (&CscCigHandleMemPool[instCnt][0] == hObjs)
        {
            /* Check if the memory is already allocated */
            GT_assert(CscCigCoreTrace, (TRUE == CscCigHandleMemFlag[instCnt]));
            CscCigHandleMemFlag[instCnt] = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }
    Semaphore_post(CscCigPoolSem);

    return (retVal);
}

/**
 *  cscCigAllocHandleObj
 *  Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
static Vcore_CscCigHandleObj *cscCigAllocHandleObj(UInt32 instId,
                                        Vcore_OpMode mode)
{
    UInt32              instCnt, handleCnt;
    UInt32              cookie;
    Vcore_CscCigInstObj    *instObj = NULL;
    Vcore_CscCigHandleObj      *hObj = NULL;

    /* Find out the instance to which this handle belongs to. */
    for (instCnt = 0u; instCnt < CscCigProperty.numInstance; instCnt++)
    {
        if (CscCigInstObjects[instCnt].instId == instId)
        {
            instObj = &CscCigInstObjects[instCnt];
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
                GT_0trace(CscCigCoreTrace, GT_ERR,
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
            if (FALSE == instObj->hObjs[handleCnt].used)
            {
                hObj = &instObj->hObjs[handleCnt];
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
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);
    return (hObj);
}

/**
 *  cscCigAllocChannelMem
 *  Allocate memory for channel object from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_CscCigChObj *cscCigAllocChannelMem(void)
{
    UInt32              channelCnt;
    Vcore_CscCigChObj *chObj = NULL;

    Semaphore_pend(CscCigPoolSem, VCORE_CSC_CIG_SEM_TIMEOUT);

    for (channelCnt = 0u; channelCnt < VCORE_CSC_CIG_MAX_CHANNEL_OBJECTS;
         channelCnt++)
    {
        if (FALSE == CscCigChannelMemPool[channelCnt].isAllocated)
        {
            chObj = &CscCigChannelMemPool[channelCnt];
            CscCigChannelMemPool[channelCnt].isAllocated = TRUE;
            break;
        }
    }
    if (NULL == chObj)
    {
        GT_0trace(CscCigCoreTrace, GT_ERR,
            "Channel Object memory alloc failed!!\n");
    }
    Semaphore_post(CscCigPoolSem);
    return (chObj);
}

/**
 *  cscCigSetDefaultChInfo
 *  Resets the channel information with default values.
 */
static void cscCigSetDefaultChInfo(Vcore_CscCigChObj *chObj,
                                UInt32 channelCnt)
{
    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isFormatSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /* Initialize format structure - Assumes YUV422 720x480 progressive */
    chObj->coreFmt.frameWidth = VCORE_CSC_CIG_DEFAULT_WIDTH;
    chObj->coreFmt.frameHeight = VCORE_CSC_CIG_DEFAULT_HEIGHT;
    chObj->coreFmt.startX = 0u;
    chObj->coreFmt.startY = 0u;
    chObj->coreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.memType = (UInt32) VPSHAL_VPDMA_MT_NONTILEDMEM;

    chObj->coreFmt.fmt.channelNum = channelCnt;
    chObj->coreFmt.fmt.width = VCORE_CSC_CIG_DEFAULT_WIDTH;
    chObj->coreFmt.fmt.height = VCORE_CSC_CIG_DEFAULT_HEIGHT;
    chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_CSC_CIG_DEFAULT_WIDTH * 2u;
    chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chObj->coreFmt.fmt.dataFormat = FVID2_DF_YUV422I_YUYV;
    chObj->coreFmt.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.fmt.bpp = FVID2_BPP_BITS16;
    chObj->coreFmt.fmt.reserved = NULL;
#if 0
    /* Initialize channel info structure */
    chObj->descInfo.numInDataDesc = 0;
    chObj->descInfo.numOutDataDesc = 0;
    chObj->descInfo.shadowOvlySize = 0;
    chObj->descInfo.nonShadowOvlySize = 0;
    chObj->descInfo.coeffOvlySize = 0;

    /* Initialize mosaic info structure */
    chObj->numExtraDesc = 0u;
    chObj->isDummyNeeded = FALSE;
#endif
    return;
}

/**
 *  cscCigFreeChannelMem
 *  Frees-up the channel object memory.
 */
static Int32 cscCigFreeChannelMem(const Vcore_CscCigChObj *chObj)
{
    UInt32              channelCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != chObj));

    Semaphore_pend(CscCigPoolSem, VCORE_CSC_CIG_SEM_TIMEOUT);

    for (channelCnt = 0; channelCnt < VCORE_CSC_CIG_MAX_CHANNEL_OBJECTS;
         channelCnt++)
    {
        if (&CscCigChannelMemPool[channelCnt] == chObj)
        {
            /* Check if the memory is already allocated */
            GT_assert(CscCigCoreTrace, (TRUE ==
                CscCigChannelMemPool[channelCnt].isAllocated));
            CscCigChannelMemPool[channelCnt].isAllocated = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }

    Semaphore_post(CscCigPoolSem);

    return (retVal);
}

/**
 *  cscCigFreeChannelObj
 *  Frees-up the channel objects.
 */
static Int32 cscCigFreeChannelObj(Vcore_CscCigChObj **chObjs,
                              UInt32 numCh)
{
    UInt32              channelCnt;
    Int32               retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != chObjs));

    for (channelCnt = 0; channelCnt < numCh; channelCnt++)
    {
        /* Memset the channel object before freeing */
        retVal += cscCigFreeChannelMem(chObjs[channelCnt]);
        if (VPS_SOK == retVal)
        {
            VpsUtils_memset(chObjs[channelCnt], 0, sizeof(Vcore_CscCigChObj));
        }
        chObjs[channelCnt] = NULL;
    }

    return (retVal);
}

/**
 *  cscCigAllocChannelObj
 *  Allocates channel objects.
 *  Returns error if allocation failed.
 */
static Int32 cscCigAllocChannelObj(Vcore_CscCigChObj **chObjs,
                               UInt32 numCh)
{
    UInt32      channelCnt, errCnt;
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != chObjs));

    for (channelCnt = 0u; channelCnt < numCh; channelCnt++)
    {
        /* Allocate channel objects one at a time */
        chObjs[channelCnt] = cscCigAllocChannelMem();
        if (NULL == chObjs[channelCnt])
        {
            GT_1trace(CscCigCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                channelCnt);
            retVal = VPS_EFAIL;
            break;
        }

        /* Set default values for the channel object */
        cscCigSetDefaultChInfo(chObjs[channelCnt], channelCnt);
    }

    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        for (errCnt = 0u; errCnt < channelCnt; errCnt++)
        {
            cscCigFreeChannelMem(chObjs[errCnt]);
            chObjs[errCnt] = NULL;
        }
    }

    return (retVal);
}

/**
 *  cscCigFreeHandleObj
 *  Frees-up the handle object and resets the variables.
 */
static Int32 cscCigFreeHandleObj(Vcore_CscCigHandleObj *hObj)
{
    UInt32              handleCnt;
    UInt32              cookie;
    Int32               retVal = VPS_EFAIL;
    Vcore_CscCigInstObj    *instObj;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(CscCigCoreTrace, (NULL != instObj));

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Free the handle object */
    for (handleCnt = 0u; handleCnt < instObj->numHandle; handleCnt++)
    {
        if (hObj == &instObj->hObjs[handleCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(CscCigCoreTrace, (TRUE == instObj->hObjs[handleCnt].used));

            /* Reset the variables */
            hObj->used = FALSE;
            hObj->parent = NULL;
            hObj->perChCfg = FALSE;
            hObj->numCh = 0u;
            cscCigSetDefaultChInfo(&hObj->handleContext, 0u);

            /* If last handle of the instance, then set mode to invalid */
            if (1 == instObj->openCnt)
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
 *  cscCigGetChObj
 *  Returns the channel object pointer depending on per channel config is
 *  enabled or not.
 */
static Vcore_CscCigChObj *cscCigGetChObj(Vcore_CscCigHandleObj *hObj, UInt32 chNum)
{
    Vcore_CscCigChObj  *chObj;

    /* NULL pointer check */
    GT_assert(CscCigCoreTrace, (NULL != hObj));

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
 *  Vcore_cscCigUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       [IN] CSC+CIG Path Core handle.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frame        [IN] Pointer to the FVID frame containing the buffer
 *                      address. This parameter should be non-NULL.
 *  \param fid          [IN] FID to be programmed in the descriptor.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_cscCigUpdateDesc(Vcore_Handle handle,
                                const Vcore_DescMem *descMem,
                                const FVID2_Frame *frame,
                                UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    return (retVal);
}

