/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_secPath.c
 *
 *  \brief VPS secondadry path core file.
 *  This file implements the core layer for the secondary paths
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
#include <ti/psp/vps/hal/vpshal_chrus.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_secPath.h>
#include <ti/psp/platforms/vps_platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief
 * Maximum number of handles supported per secondary path instance.
 */
#define VCORE_SEC_MAX_HANDLES           (4u)

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_SEC_MAX_HANDLES *
 *  VCORE_SEC_MAX_CHANNELS_PER_INST objects per instance,
 *  only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_SEC_MAX_CHANNEL_OBJECTS    (VCORE_SEC_MAX_CHANNELS_PER_INST * \
                                          VCORE_SEC_NUM_INST)

/** \brief Time out to be used in sem pend */
#define VCORE_SEC_SEM_TIMEOUT            (BIOS_WAIT_FOREVER)

/** \brief Default width used for initializing format structure.*/
#define VCORE_SEC_DEFAULT_WIDTH          (720u)

/** \brief Default height used for initializing format structure. */
#define VCORE_SEC_DEFAULT_HEIGHT         (480u)

/** \brief Maximum VPDMA channels to be programmed  */
#define VCORE_SEC_MAX_VPDMA_CHANNELS (2u)

/**\brief Y Channel index */
#define VCORE_SEC_Y_CHANNEL_IDX      (0u)

/**\brief Y Channel index */
#define VCORE_SEC_CBCR_CHANNEL_IDX   (1u)

/**
 *  \brief Number of descriptors needed per Buffer.
 */
#define VCORE_SEC_NUM_DESC_PER_WINDOW    (2u)

/** \brief Number of VPDMA frame start event regsiter. */
#define VCORE_SEC_NUM_VPDMA_FS_EVT_REG  (2u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief
 * Typedef for secondary path instance object.
 */
typedef struct Vcore_SecInstObj_t Vcore_SecInstObj;

/**
 *  struct Vcore_SecChObj
 *  \brief Structure containing per channel information.
 */
typedef struct
{
    Vcore_States            state;
    /**< Current state */
    Vcore_Format            coreFmt;
    /**< Buffer formats and other formats */
    Vcore_DescInfo          descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data descriptor memories required by this core. */
    VpsHal_ChrusConfig      chrusConfig;
    /**< Chroma upsampler configuration.  This is to pass to the chroma
         up-sampler HAL.  It will be configured once during handle open.
         After that only relevant info will be configure for each queue. */
    UInt32                  chrusConfigDone;
    /**< Flag to indicate whether the chroma up sampler configuration params
         are done */
    UInt32                  bufIndex[VCORE_SEC_NUM_DESC_PER_WINDOW];
    /**< buffer Indexes based on the data Format*/
    UInt32                  numExtraDesc;
    /**< Number of extra descriptors needed at the end of the frame i.e. after
         the last mosaic data descriptor. This includes dummy data descriptor,
         SOCH descriptor on actual or last window free channel, abort
         descriptor on actual channel and abort descriptors on free channel of
         last window in all rows. Used in non-mosaic configuration. */
    UInt32                  isDummyNeeded;
    /**< Specifies if a dummy channel would be required as window size
         is less than that of the frame boundary. */
    UInt32                  isAllocated;
    /**< Flag to  indicate channel object is allocated or not */
    VpsHal_VpdmaFSEvent     fsEvent;
    /**< Frame start event for this channel */
} Vcore_SecChObj;

/**
 *  struct Vcore_SecHandleObj
 *  \brief Structure containing per handle information.
 */
typedef struct
{
    UInt32                  used;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_SecInstObj        *parent;
    /**< Pointer to the instance object. */
    UInt32                  perChCfg;
    /**< Flag indicating whether format is common for all the channel or
         it is different for each channel. */
    Vcore_SecChObj          handleContext;
    /**< Used when the handle is opened with perChCfg as false.
         Even in this mode, the data descriptor information is not updated
         in this variable - still the channel objects is used for this
         purpose. */
    UInt32                  numCh;
    /**< Number of channel for this handle. This determines the number of
         valid pointers in above channel array. */
    Vcore_SecChObj          *chObjs[VCORE_SEC_MAX_CHANNELS_PER_INST];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool */
} Vcore_SecHandleObj;

/**
 *  struct Vcore_SecInstObj_t
 *  \brief Structure containing per instance information.
 */
struct Vcore_SecInstObj_t
{
    UInt32                  instId;
    /**< Instance number/ID. */
    UInt32                  openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    VpsHal_VpdmaChannel     channels[VCORE_SEC_MAX_VPDMA_CHANNELS];
    /**< 420T/422T secondary path channels.  It has two VPDMA channels. */
    Vcore_OpMode            curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or mem-mem operation of the core. */
    Vcore_SecHandleObj      *hObjs;
    /**< Pointer to the handle information. This is a contiguous memory of
         numHandles Vcore_SecHandleObj structure */
    UInt32                  numHandle;
    /**< Number of handles for this instance. This determines the size of
         handle array. */
    VpsHal_Handle           chrusHandle;
    /**< Handle of the Chroma up-sampler HAL.  This will be used to do
         configure the Chroma up-sampler. */
    UInt32                  *fsRegOffset[VCORE_SEC_NUM_VPDMA_FS_EVT_REG];
    /**< To store the VPDMA client register address for the luma and chroma
         channels of the chroma upsampler. This will be used only under
         MEM_MEM mode as under display mode VPDMA client are updated through
         registers only instead of overlay pointers */
    UInt32                  virtRegOffset[2];
    /**< To store the virtual register index in the overlay memory pointer
         This will be used only under MEM_MEM mode as under display mode VPDMA
         client are updated through registers only instead of overlay
         pointers */
    VpsHal_VpsClkcModule    module;
    /**< Module enum for initializing the clock */
    VpsHal_VpdmaPath        vpdmaPath;
    /**< Vpdma Path */
    UInt32                  numVpdmaCh;
    /**< Number of vpdma channels supported by this path */
    UInt32                  vpdmaChNum[VCORE_SEC_MAX_VPDMA_CHANNELS];
    /**< Channel number for each vpdma channels */
};

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


static Vcore_Handle Vcore_secOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg);
static Int32 Vcore_secClose(Vcore_Handle handle);
static Int32 Vcore_secSetFormat(Vcore_Handle handle,
                                UInt32 channel,
                                const Vcore_Format *coreFmt);
static Int32 Vcore_secGetProperty(Vcore_Handle handle,
                                 Vcore_Property *property);
static Int32 Vcore_secGetFormat(Vcore_Handle handle,
                                UInt32 channel,
                                Vcore_Format *coreFmt);
static Int32 Vcore_secGetDescInfo(Vcore_Handle handle,
                                  UInt32 channel,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo);
static Int32 Vcore_secSetFSEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_secProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem);
static Int32 Vcore_secProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_secUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid);
static Int32 Vcore_secUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtParams,
                                 UInt32 frmIdx);

static Int32 secCheckFormat(const Vcore_SecHandleObj *hObj,
                            const Vcore_Format *coreFmt);

static Int32 secUpdateLineMode (Vcore_SecHandleObj *hObj,
                                Vcore_SecChObj *chObj,
                                const Vcore_DescMem *descMem);
static Vcore_SecHandleObj *secAllocHandleObj(UInt32 instId,
                                             Vcore_OpMode mode);

static Int32 secFreeHandleObj(Vcore_SecHandleObj *hObj);

static Int32 secAllocChannelObj(Vcore_SecChObj **chObjs,
                                UInt32 numCh);

static Int32 secFreeChannelObj(Vcore_SecChObj **chObjs,
                               UInt32 numCh);

static void secSetDefaultChInfo(Vcore_SecChObj *chObj,
                                 UInt32 channelCnt);

static Vcore_SecHandleObj *secAllocHandleMem(UInt32 numHandle);

static Int32 secFreeHandleMem(const Vcore_SecHandleObj *hObjs);

static Vcore_SecChObj *secAllocChannelMem(void);

static Int32 secFreeChannelMem(const Vcore_SecChObj *chObj);

static UInt32 secProgramDataDesc(const Vcore_SecHandleObj *hObj,
                                 Vcore_SecChObj *chObj,
                                 const Vcore_DescMem *descMem);
static UInt32 secProgramReg(const Vcore_SecHandleObj *hObj,
                               Vcore_SecChObj *chObj,
                              const Vcore_DescMem *descMem);

static Int32 secUpdateRtDescMem(const Vcore_SecHandleObj *hObj,
                                Vcore_SecChObj *chObj,
                                const Vcore_DescMem *descMem);
static Int32 secUpdateRtParams(const Vcore_SecHandleObj *hObj,
                               Vcore_SecChObj *chObj,
                               const Vcore_SecRtParams *rtParams);
static Vcore_SecChObj *secGetChObj(Vcore_SecHandleObj *hObj, UInt32 chNum);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief Secondary Path Core function pointer. */
static const Vcore_Ops SecCoreOps =
{
    Vcore_secGetProperty,               /* Get property */
    Vcore_secOpen,                      /* Open function */
    Vcore_secClose,                     /* Close function */
    Vcore_secSetFormat,                 /* Set format */
    Vcore_secGetFormat,                 /* Get format */
    NULL,                               /* Set mosaic */
    NULL,                               /* Create layout */
    NULL,                               /* Delete layout */
    Vcore_secGetDescInfo,               /* Get descriptor */
    Vcore_secSetFSEvent,                /* Frame start event for Handle */
    NULL,                               /* Set params */
    NULL,                               /* Get params */
    NULL,                               /* Control */
    Vcore_secProgramDesc,               /* Program Descriptors */
    Vcore_secProgramReg,                /* Program registors/overlay memory */
    Vcore_secUpdateDesc,                /* Update the descriptors */
    NULL,                               /* Update Multidesc */
    NULL,                               /* Update context */
    NULL,                               /* Get DEI ctx info funct pointer. */
    Vcore_secUpdateRtMem,               /* Update run time configuration */
    NULL                                /* Program scalar coefficient */
};
/** \brief Secondary path objects */
static Vcore_SecInstObj SecInstObjects[VCORE_SEC_NUM_INST];

/** \brief Properties of this core */
static Vcore_Property SecProperty;

/**
 *  \brief Memory pool for the Handle objects per instance.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_SecHandleObj SecHandleMemPool
            [VCORE_SEC_NUM_INST][VCORE_SEC_MAX_HANDLES];

/**
 *  \brief The flag variable represents whether a given handle object is
 *  allocated or not
 */
static UInt32 SecHandleMemFlag[VCORE_SEC_NUM_INST];

/**
 *  \brief Memory pool for the channel objects.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_SecChObj SecChannelMemPool[VCORE_SEC_MAX_CHANNEL_OBJECTS];

/** \brief Semaphore for protecting allocation and freeing of memory pool
    objects */
static Semaphore_Handle SecPoolSem = NULL;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_secInit
 *  \brief Secondary Path Core init function.
 *  Initializes Secondary Path core objects, allocates memory etc.
 *  This function should be called before calling any of SEC core API's.
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
Int32 Vcore_secInit(UInt32 numInstance,
                   const Vcore_SecInitParams *initParams,
                   Ptr arg)
{
    UInt32              instCnt, handleCnt;
    Vcore_SecInstObj    *instObj;
    Int32               retVal = VPS_SOK;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(SecCoreTrace, (numInstance <= VCORE_SEC_NUM_INST));
    GT_assert(SecCoreTrace, (NULL != initParams));

    /* Initialize sec Objects to zero */
    VpsUtils_memset(SecInstObjects, 0, sizeof(SecInstObjects));
    VpsUtils_memset(&SecProperty, 0, sizeof(SecProperty));
    VpsUtils_memset(&SecHandleMemPool, 0, sizeof(SecHandleMemPool));
    VpsUtils_memset(&SecChannelMemPool, 0, sizeof(SecChannelMemPool));

    /* Mark pool flags as free */
    for (instCnt = 0u; instCnt < VCORE_SEC_NUM_INST; instCnt++)
    {
        SecHandleMemFlag[instCnt] = FALSE;
        for (handleCnt = 0u; handleCnt < VCORE_SEC_MAX_HANDLES; handleCnt++)
        {
            SecHandleMemPool[instCnt][handleCnt].used = FALSE;
        }
    }

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    SecPoolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == SecPoolSem)
    {
        GT_0trace(SecCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        /* Initialize secondary  path core properties */
        SecProperty.numInstance = numInstance;
        SecProperty.name = VCORE_TYPE_SEC;
        SecProperty.type = VCORE_TYPE_INPUT;
        SecProperty.internalContext = FALSE;
        for (instCnt = 0u; instCnt < numInstance; instCnt++)
        {
            /* Initialize instance object members */
            instObj = &SecInstObjects[instCnt];
            instObj->instId = initParams[instCnt].instId;
            instObj->channels[VCORE_SEC_Y_CHANNEL_IDX] =
                initParams[instCnt].vpdmaCh[VCORE_SEC_Y_CHANNEL_IDX];
            instObj->channels[VCORE_SEC_CBCR_CHANNEL_IDX] =
                initParams[instCnt].vpdmaCh[VCORE_SEC_CBCR_CHANNEL_IDX];
            instObj->chrusHandle = initParams[instCnt].chrusHandle;
            instObj->curMode = VCORE_OPMODE_INVALID;
            instObj->module = initParams[instCnt].module;
            instObj->vpdmaPath = initParams[instCnt].vpdmaPath;
            instObj->numVpdmaCh = VCORE_SEC_MAX_VPDMA_CHANNELS;
            /* Allocate handle object memory */
            instObj->hObjs = secAllocHandleMem(
                                  initParams[instCnt].maxHandle);
            if (NULL == instObj->hObjs)
            {
                GT_1trace(SecCoreTrace, GT_ERR,
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
        Vcore_secDeInit(NULL);
    }
    return (retVal);
}

/**
 *  Vcore_secDeInit
 *  \brief Secondary Path exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_secDeInit(Ptr arg)
{
    UInt32              instCnt;
    Vcore_SecInstObj    *instObj;

    /* Free-up handle objects for each instance */
    for (instCnt = 0u; instCnt < SecProperty.numInstance; instCnt++)
    {
        instObj = &SecInstObjects[instCnt];
        instObj->instId = VCORE_SEC_INST_0;
        instObj->channels[VCORE_SEC_Y_CHANNEL_IDX] = (VpsHal_VpdmaChannel) 0;
        instObj->channels[VCORE_SEC_CBCR_CHANNEL_IDX] = (VpsHal_VpdmaChannel) 0;
        instObj->curMode = VCORE_OPMODE_INVALID;
        if (instObj->hObjs)
        {
            secFreeHandleMem(instObj->hObjs);
            instObj->hObjs = NULL;
            instObj->numHandle = 0u;
        }
    }
    SecProperty.numInstance = 0u;

    /* Delete the Semaphore created for Pool objects */
    if (SecPoolSem)
    {
        Semaphore_delete(&SecPoolSem);
        SecPoolSem = NULL;
    }

    return (VPS_SOK);
}

/**
 *  Vcore_secGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_secGetCoreOps(void)
{
    return &SecCoreOps;

}

/**
 *  Vcore_secOpen
 *  \brief Opens a particular instance of secondary in either
 *  display or mem-mem mode and returns handle.
 *  When opened in display mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in mem-mem mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_SEC_MAX_HANDLES macro. Also each of the handle could have multiple
 *  channel to support in a single handle.  The maximum number of channels for
 *  all the handles is determined by
 *
 *  \param instId       Instance to open - Indpendent secondary path 0
 *                      or  Secondary Path 1.
 *  \param mode         Mode to open the instance for - display or mem-mem.
 *  \param numCh   Number of channel to be associated with this open.
 *                      For display operation only one channel is allowed.
 *                      For mem-mem operation, more than one channel could
 *                      be associated with a handle handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_SEC_MAX_CHANNELS_PER_INST macro.
 *  \param perChCfg Flag indicating whether the config descriptors
 *                          should be separate for each and every channel or
 *                          not. This is valid only for mem-mem drivers. For
 *                          display drivers this should be set to FALSE.
 *                          TRUE - Separate config descriptors for each channel.
 *                          FALSE - One config descriptors used per handle.
 *
 *  \return             If success returns the handle else returns NULL.
 */
static Vcore_Handle Vcore_secOpen(UInt32 instId,
                          Vcore_OpMode mode,
                          UInt32 numCh,
                          UInt32 perChCfg)
{
    Vcore_SecHandleObj      *hObj = NULL;
    Int32               retVal = VPS_SOK;
    Vcore_SecInstObj    *instObj = NULL;
    UInt32              instCnt;

    /* Only memory mode supported for Instance 0*/
    if (instId == VCORE_SEC_INST_0)
    {
        GT_assert(SecCoreTrace, (VCORE_OPMODE_MEMORY == mode));
    }
    /* Mem-Mem and display both supported for Instance 1 */
    else
    {
        GT_assert(SecCoreTrace,
            (VCORE_OPMODE_MEMORY == mode || VCORE_OPMODE_DISPLAY == mode));
    }
    /* Check for maximum channel supported per handle */
    GT_assert(SecCoreTrace, (numCh <= VCORE_SEC_MAX_CHANNELS_PER_INST));
    /* There should be atleast one channel */
    GT_assert(SecCoreTrace, (0u != numCh));

    /* For display operation, only one channel should be allowed */
    GT_assert(SecCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (VCORE_MAX_DISPLAY_CH < numCh))));

    /* For display operation, per channel format should be FALSE */
    GT_assert(SecCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (FALSE != perChCfg))));

    for (instCnt = 0u; instCnt < SecProperty.numInstance; instCnt++)
    {
        if (SecInstObjects[instCnt].instId == instId)
        {
            instObj = &SecInstObjects[instCnt];
            break;
        }
    }
    if (VPS_SOK == retVal && (NULL != instObj))
    {
        /* Allocate a handle object */
        hObj = secAllocHandleObj(instId, mode);
        if (NULL != hObj)
        {
            /* Allocate channel objects */
            retVal = secAllocChannelObj(hObj->chObjs, numCh);
            if (VPS_SOK != retVal)
            {
                GT_0trace(SecCoreTrace, GT_ERR, "Channel allocation failed!!\n");
                /* Free-up handle object if channel allocation failed */
                secFreeHandleObj(hObj);
                hObj = NULL;
            }
            else
            {
                /* Initialize variables */
                hObj->perChCfg = perChCfg;
                hObj->numCh = numCh;
                secSetDefaultChInfo(&hObj->handleContext, 0u);
            }
        }
        else
        {
            GT_0trace(SecCoreTrace, GT_ERR, "Handle allocation failed!!\n");
        }
    }

    if (NULL != hObj && NULL != instObj)
    {
        if (0 == instObj->openCnt && NULL != instObj)
        {
            retVal = VpsHal_vpsClkcModuleEnable(instObj->module, TRUE);

            if (VPS_SOK != retVal)
            {
                secFreeChannelObj(hObj->chObjs, hObj->numCh);
                secFreeHandleObj(hObj);
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
 *  Vcore_secClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       Secondary path handle.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_secClose(Vcore_Handle handle)
{
    Int32                   retVal = VPS_EFAIL;
    Vcore_SecHandleObj      *hObj;
    Vcore_SecInstObj        *instObj = NULL;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));

    hObj = (Vcore_SecHandleObj *) handle;

    instObj = hObj->parent;
    GT_assert(SecCoreTrace, (NULL != instObj));
    /* Free channel objects */
    retVal = secFreeChannelObj(hObj->chObjs, hObj->numCh);
    /* Free handle objects */
    retVal += secFreeHandleObj(hObj);
    /* Null the handle for sake of completeness */
    hObj = NULL;
    Semaphore_pend(SecPoolSem, VCORE_SEC_SEM_TIMEOUT);
    /* Decrement the count by one */
    instObj->openCnt--;
    if (0 == instObj->openCnt)
    {
        retVal += VpsHal_vpsClkcModuleEnable(instObj->module, FALSE);
    }
    Semaphore_post(SecPoolSem);
    return (retVal);
}

/**
 *  Vcore_SecGetProperty
 *  \brief Gets the core properties of the secondary path core.
 *
 *  \param handle       Secondary Path Core handle.
 *  \param property     Pointer to which the secondary path core properties to
 *                      be copied.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_secGetProperty(Vcore_Handle handle,
                                 Vcore_Property *property)
{
    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != property));

    VpsUtils_memcpy(property, &SecProperty, sizeof(Vcore_Property));

    return (VPS_SOK);
}

/**
 *  Vcore_secSetFormat.
 *  \brief sets the format for the core. Initializes how many data and config
 *  descriptors will be required depending upon the format passed.
 *
 *  \param handle       Secondary Path handle.
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
static Int32 Vcore_secSetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_Format *coreFmt)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SecHandleObj      *hObj;
    Vcore_SecChObj          *chObj;
    UInt32                  vpdmaChCnt;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_SecHandleObj *) handle;

     /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    /* Check if the channel is within the range allocated during open */
    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SecCoreTrace, (NULL != chObj));

        /* Check whether the requested format is valid */
        retVal = secCheckFormat(hObj, coreFmt);
        if (VPS_SOK != retVal)
        {
            GT_0trace(SecCoreTrace, GT_ERR, "Invalid Format!!\n");
        }
    }
    if (VPS_SOK == retVal)
    {
        /* Dummy data descriptor is not needed if frame size and buffer
         * bottom-right coordinates match */
        chObj->numExtraDesc = 0u;
        if (((coreFmt->startX + coreFmt->fmt.width) < coreFmt->frameWidth) ||
            ((coreFmt->startY + coreFmt->fmt.height) < coreFmt->frameHeight))
        {
            chObj->isDummyNeeded = TRUE;
            chObj->numExtraDesc += VCORE_SEC_NUM_DESC_PER_WINDOW;
        }
        else
        {
            chObj->isDummyNeeded = FALSE;
            if (VCORE_OPMODE_DISPLAY == hObj->parent->curMode)
            {
                /* Always assume dummy is required for display mode as
                 * during runtime it might be needed when width/height or
                 * startX/startY changes. */
                chObj->numExtraDesc += VCORE_SEC_NUM_DESC_PER_WINDOW;
            }
        }

        /* Add data descriptor for SOCH and abort of actual channel
         * in display mode */
        if (VCORE_OPMODE_DISPLAY == hObj->parent->curMode)
        {
            chObj->numExtraDesc += VCORE_SEC_NUM_DESC_PER_WINDOW;
        }

        /* Two data descriptors will be required for chroma up sampler
         * in any case */
        chObj->descInfo.numInDataDesc = VCORE_SEC_NUM_DESC_PER_WINDOW;
        chObj->descInfo.numOutDataDesc = 0u;
        chObj->descInfo.horzCoeffOvlySize = 0u;
        chObj->descInfo.vertCoeffOvlySize = 0u;
        chObj->descInfo.vertBilinearCoeffOvlySize = 0u;
        chObj->descInfo.numChannels = hObj->parent->numVpdmaCh;
        for(vpdmaChCnt = 0; vpdmaChCnt < hObj->parent->numVpdmaCh; vpdmaChCnt++)
        {
            chObj->descInfo.socChNum[vpdmaChCnt] =
                hObj->parent->channels[vpdmaChCnt];
        }

        chObj->descInfo.numMultiWinDataDesc = chObj->numExtraDesc;
        if (VCORE_OPMODE_MEMORY == hObj->parent->curMode)
        {
            /* Get the overlay memory size of the chroma up sampler */
            chObj->descInfo.shadowOvlySize =
                VpsHal_chrusGetConfigOvlySize(hObj->parent->chrusHandle);
        }
        else
        {
            chObj->descInfo.shadowOvlySize = 0u;
        }

        if (VCORE_OPMODE_MEMORY == hObj->parent->curMode)
        {
            /* Get the base address for the client register for the luma
            channel It will be one word plus for the chorma channel*/
            hObj->parent->fsRegOffset[0] =
            VpsHal_vpdmaGetClientRegAdd(
                hObj->parent->channels[VCORE_SEC_Y_CHANNEL_IDX]);

            hObj->parent->fsRegOffset[1] =
            VpsHal_vpdmaGetClientRegAdd
            (hObj->parent->channels[VCORE_SEC_CBCR_CHANNEL_IDX]);
            /* Get the overlay memory size required and
             * virtual register address for that client register to
             * program it into overlay memory */
            chObj->descInfo.nonShadowOvlySize =
                VpsHal_vpdmaCalcRegOvlyMemSize(hObj->parent->fsRegOffset,
                    VCORE_SEC_NUM_VPDMA_FS_EVT_REG,
                    hObj->parent->virtRegOffset);
        }
        else
        {
            chObj->descInfo.nonShadowOvlySize = 0;
        }
        /* Copy the format to the local structure */
        VpsUtils_memcpy(&chObj->coreFmt,
            (Ptr) coreFmt,
            sizeof(Vcore_Format));
        if ((FVID2_DF_YUV420SP_UV == chObj->coreFmt.fmt.dataFormat) ||
                (FVID2_DF_YUV422SP_UV == chObj->coreFmt.fmt.dataFormat))
        {
            chObj->bufIndex[0] = FVID2_YUV_SP_Y_ADDR_IDX;
            chObj->bufIndex[1] = FVID2_YUV_SP_CBCR_ADDR_IDX;
        }
        else
        {
            chObj->bufIndex[0] = FVID2_YUV_INT_ADDR_IDX;
            chObj->bufIndex[1] = FVID2_YUV_INT_ADDR_IDX;
        }
        /* Set proper states - since format is set, desc info should be set
         * again */
        chObj->state.isFormatSet = TRUE;

    }
    return (retVal);
}

/**
 *  Vcore_secGetFormat
 *  \brief Gets the format for a given channel.
 *
 *  \param handle       Secondary Path handle.
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
static Int32 Vcore_secGetFormat(Vcore_Handle handle,
                        UInt32 chNum,
                        Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
    Vcore_SecHandleObj      *hObj;
    Vcore_SecChObj *chObj;

     /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_SecHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SecCoreTrace, (NULL != chObj));

        /* Copy the format */
        VpsUtils_memcpy((Ptr)coreFmt, &chObj->coreFmt, sizeof(Vcore_Format));
    }

    return (retVal);
}

/**
 *  Vcore_secGetDescInfo
 *  \brief Depending on the format set, returns the number of data
 *  descriptors needed for the actual driver to allocate memory.
 *  Format should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       Secondary Path handle.
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
static Int32 Vcore_secGetDescInfo(Vcore_Handle handle,
                              UInt32 chNum,
                              UInt32 layoutId,
                              Vcore_DescInfo *descInfo)
{
    Int32               retVal = VPS_SOK;
    Vcore_SecHandleObj      *hObj;
    Vcore_SecChObj *chObj;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != descInfo));

    hObj = (Vcore_SecHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);

        if (TRUE == chObj->state.isFormatSet)
        {
            retVal = VPS_SOK;
            /* Copy the data descriptor information */
            VpsUtils_memcpy(
                (Ptr) descInfo,
                &chObj->descInfo,
                sizeof(Vcore_DescInfo));
        }
        else
        {
             retVal = VPS_EFAIL;
            GT_0trace(SecCoreTrace, GT_ERR,
                "Set the parameter before getting the descriptor info!!\n");
        }
    }
    return (retVal);
}

/**
 *  Vcore_secSetFSEvent
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
static Int32 Vcore_secSetFSEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem)
{
     Int32               retVal = VPS_SOK;
     Vcore_SecHandleObj      *hObj;

     Vcore_SecChObj *chObj;


    /* NULL pointer check */
    GT_assert(curTrace, (NULL != handle));


    hObj = (Vcore_SecHandleObj *) handle;

    chObj = secGetChObj(hObj, chNum);
    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != chObj));

    chObj->fsEvent = fsEvent;

    retVal = secUpdateLineMode (hObj, chObj, descMem);

    return (retVal);
}

/**
 *  Vcore_secProgramDesc
 *  \brief Programs the data descriptor for a given channel. This will not
 *  program the internal context descriptors.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       Secondary Path Core handle.
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
static Int32 Vcore_secProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SecHandleObj      *hObj;
    Vcore_SecChObj     *chObj;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != descMem));

    hObj = (Vcore_SecHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SecCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SecCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }
    if (VPS_SOK == retVal)
    {
        /* Configure data descriptors */
        retVal = secProgramDataDesc(hObj, chObj, descMem);
    }
    return (retVal);
}

/**
 *  Vcore_secProgramReg
 *  \brief Programs the registers for HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       Secondary Path Core handle.
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
static Int32 Vcore_secProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SecHandleObj     *hObj;
    Vcore_SecChObj         *chObj;
    Vcore_SecInstObj       *instObj;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    hObj = (Vcore_SecHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(SecCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SecCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SecCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != descMem && NULL != descMem->shadowOvlyMem)
        {
            retVal = VpsHal_chrusCreateConfigOvly(hObj->parent->chrusHandle,
                descMem->shadowOvlyMem);
        }
    }

    if (VPS_SOK == retVal)
    {
        retVal = secProgramReg(hObj, chObj, descMem);
    }

    return (retVal);
}

/**
 *  Vcore_swpUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       Secondary Path Core handle.
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
static Int32 Vcore_secUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SecHandleObj     *hObj;
    Vcore_SecChObj         *chObj;
    Vcore_SecInstObj       *instObj;
    Void                   *descPtr;
    UInt32                  chNum;
    UInt32                  numDataDesc;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != descMem));
    GT_assert(SecCoreTrace, (NULL != frame));

    hObj = (Vcore_SecHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(SecCoreTrace, (NULL != instObj));

    /* For display mode, channel number is always zero. So ignore it. */
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        chNum = 0u;
    }
    else
    {
        chNum = frame->channelNum;
        /* Check if the channel is within the range allocated during open */
        if (chNum >= hObj->numCh)
        {
            retVal = VPS_EOUT_OF_RANGE;
            GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SecCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SecCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if (VPS_SOK == retVal)
    {
        for (numDataDesc = 0; numDataDesc < VCORE_SEC_NUM_DESC_PER_WINDOW;
                    numDataDesc++)
        {
            descPtr = descMem->inDataDesc[numDataDesc];
            GT_assert(SecCoreTrace, (NULL != descPtr));
            VpsHal_vpdmaSetAddress(
                descPtr,
                fid,
                frame->addr[fid][chObj->bufIndex[numDataDesc]]);
        }
    }
    return (retVal);
}


/**
 *  Vcore_secUpdateRtMem
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       Secondary Path Core handle.
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
static Int32 Vcore_secUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtParams,
                                 UInt32 frmIdx)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SecHandleObj      *hObj;
    Vcore_SecChObj     *chObj;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != handle));
    GT_assert(SecCoreTrace, (NULL != descMem));

    hObj = (Vcore_SecHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = secGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SecCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SecCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != rtParams)
        {
            retVal = secUpdateRtParams(hObj, chObj, rtParams);
            if (VPS_SOK != retVal)
            {
                GT_1trace(SecCoreTrace, GT_ERR,
                    "Update RT params failed for channel %d!!\n", chNum);
            }
        }
        if (VPS_SOK == retVal)
        {
            retVal = secUpdateRtDescMem(hObj, chObj, descMem);
            if (VPS_SOK != retVal)
            {
                GT_1trace(SecCoreTrace, GT_ERR,
                    "Update RT memory failed for channel %d!!\n", chNum);
            }
        }
        if (VPS_SOK == retVal && (VCORE_OPMODE_MEMORY == hObj->parent->curMode))
        {
            retVal = secUpdateLineMode (hObj, chObj, descMem);
            if (VPS_SOK != retVal)
            {
                GT_1trace(SecCoreTrace, GT_ERR,
                    "Update line mode failed for channel %d!!\n", chNum);
            }

        }
    }

    return (retVal);
}

/**
 *  secUpdateLineMode
 *  This function will update the line mode depending on data format.
 */
static Int32 secUpdateLineMode (Vcore_SecHandleObj *hObj,
                                Vcore_SecChObj *chObj,
                                const Vcore_DescMem *descMem)
{
    Int32     retVal = VPS_SOK;
    UInt32    dataFormat;
    VpsHal_VpdmaLineMode lumaLineMode, chrLineMode;
    Ptr nonShadowOvlyPtr;

    if (NULL == descMem)
    {
       nonShadowOvlyPtr = NULL;
    }
    else
    {
        nonShadowOvlyPtr = descMem->nonShadowOvlyMem;
        /* NULL pointer check */
        GT_assert(curTrace, (NULL != nonShadowOvlyPtr));
    }

    dataFormat = chObj->coreFmt.fmt.dataFormat;

    /* Line mode doesn't make sense for the Luma client so setting it to
     * 0 always for Luma clients */
    lumaLineMode = VPSHAL_VPDMA_LM_0;
    if (FVID2_DF_YUV422I_YUYV == dataFormat ||
        FVID2_DF_YUV422SP_UV == dataFormat)
    {
        chrLineMode = VPSHAL_VPDMA_LM_1;
    }
    else
    {
        chrLineMode = VPSHAL_VPDMA_LM_0;
    }

     VpsHal_vpdmaSetFrameStartEvent(
         hObj->parent->channels[VCORE_SEC_Y_CHANNEL_IDX],
         chObj->fsEvent,
         lumaLineMode,
         0u,
         nonShadowOvlyPtr,
         hObj->parent->virtRegOffset[0]);

     VpsHal_vpdmaSetFrameStartEvent(
         hObj->parent->channels[VCORE_SEC_CBCR_CHANNEL_IDX],
         chObj->fsEvent,
         chrLineMode,
         0u,
         nonShadowOvlyPtr,
         hObj->parent->virtRegOffset[1]);

     return (retVal);
}

/**
 *  secCheckParams
 *  Checks whether a given format is valid or not.
 *  Returns 0 if everything is fine else returns error value.
 */
static Int32 secCheckFormat(const Vcore_SecHandleObj *hObj,
                            const Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
    Vcore_SecInstObj   *instObj;

    GT_assert(SecCoreTrace, (NULL != hObj));
    GT_assert(SecCoreTrace, (NULL != coreFmt));
    instObj = hObj->parent;
    GT_assert(SecCoreTrace, (NULL != instObj));

    if (FVID2_DF_YUV420SP_UV == coreFmt->fmt.dataFormat ||
            FVID2_DF_YUV422SP_UV == coreFmt->fmt.dataFormat)
    {
        if (((coreFmt->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] <
                (coreFmt->fmt.width))) ||
        (coreFmt->fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] <
        (coreFmt->fmt.width))) {
            GT_2trace(SecCoreTrace, GT_ERR,
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
            GT_2trace(SecCoreTrace, GT_ERR,
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
        GT_1trace(SecCoreTrace, GT_ERR,
            "Buffer format (%d) not supported!!\n",
            coreFmt->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check whether window width/startX is even */
    if ((coreFmt->fmt.width & 0x01u) || (coreFmt->startX & 0x01u))
    {
        GT_2trace(SecCoreTrace, GT_ERR,
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
                GT_2trace(SecCoreTrace, GT_ERR,
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
                GT_2trace(SecCoreTrace, GT_ERR,
                    "Height(%d)/StartY(%d) can't be odd for YUV420 format!!\n",
                    coreFmt->fmt.height,
                    coreFmt->startY);
                retVal = VPS_EINVALID_PARAMS;
            }
        }
    }

    if (coreFmt->frameWidth < (coreFmt->startX + coreFmt->fmt.width))
    {
        GT_2trace(SecCoreTrace, GT_ERR,
            "Frame Width (%d) less than Width + startX (%d)!!\n",
            coreFmt->frameWidth,
            (coreFmt->startX + coreFmt->fmt.width));
        retVal = VPS_EINVALID_PARAMS;
    }
    if (coreFmt->frameHeight < (coreFmt->startY + coreFmt->fmt.height))
    {
        GT_2trace(SecCoreTrace, GT_ERR,
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
            GT_0trace(SecCoreTrace, GT_ERR, "Wrong Input Size!!\n");
        }
    }

    return (retVal);
}

/**
 * secProgramDataDesc
 * Programs the dataDesc params according to the configuraiton.
 * Finally calls the hal function to program the descriptor.
 */
static UInt32 secProgramDataDesc(const Vcore_SecHandleObj *hObj,
                                 Vcore_SecChObj *chObj,
                                 const Vcore_DescMem *descMem)
{
    Int32               retVal = VPS_EFAIL;
    Void                *descPtr;
    Bool                swapLumaChroma = FALSE;
    UInt32              cpuRev, platformId;
    UInt32              transHeight, descCount, frameHeight, cnt;
    UInt32              descOffset, tempOffset;
    VpsHal_VpdmaInDescParams    descParam[VCORE_SEC_NUM_DESC_PER_WINDOW];

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != hObj));
    GT_assert(SecCoreTrace, (NULL != chObj));

    cpuRev = Vps_platformGetCpuRev();
    platformId = Vps_platformGetId();
    /* Swap luma and chroma dummy descriptors for TI816x ES2.0 and
     * TI814x ES2.1 where line buffers are added in the chroma path to
     * support YUV422I HD input size. */
    swapLumaChroma = FALSE;
    if (((VPS_PLATFORM_ID_EVM_TI816x == platformId) &&
            (cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
        ((VPS_PLATFORM_ID_EVM_TI814x == platformId) &&
            (cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
        (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
    {
        swapLumaChroma = TRUE;
    }

    for (descCount = 0; descCount < chObj->descInfo.numInDataDesc; descCount++)
    {
        /* Set the desc params common for all the channels */
        descParam[descCount].transWidth = chObj->coreFmt.fmt.width;
        descParam[descCount].frameWidth = chObj->coreFmt.frameWidth;
        descParam[descCount].channel = hObj->parent->channels[descCount];
        descParam[descCount].nextChannel =  descParam[descCount].channel;
        descParam[descCount].lineStride =
            chObj->coreFmt.fmt.pitch[chObj->bufIndex[descCount]];
        descParam[descCount].startX = chObj->coreFmt.startX;

        descParam[descCount].notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == hObj->parent->curMode)
        {
            descParam[descCount].priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            descParam[descCount].priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
        }
        descParam[descCount].memType = (VpsHal_VpdmaMemoryType)
            chObj->coreFmt.memType;
        descParam[descCount].is1DMem = FALSE;
        transHeight = chObj->coreFmt.fmt.height;
        frameHeight = chObj->coreFmt.frameHeight;
        if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
        {
            descParam[descCount].transHeight = transHeight >> 1u;
            descParam[descCount].frameHeight = frameHeight >> 1u;
            descParam[descCount].startY = chObj->coreFmt.startY >> 1u;
            if (TRUE ==
                chObj->coreFmt.fmt.fieldMerged[chObj->bufIndex[descCount]])
            {
                descParam[descCount].lineSkip = VPSHAL_VPDMA_LS_2;
            }
            else
            {
                descParam[descCount].lineSkip = VPSHAL_VPDMA_LS_1;
            }
        }
        else
        {
            descParam[descCount].transHeight = transHeight;
            descParam[descCount].frameHeight = frameHeight;
            descParam[descCount].lineSkip = VPSHAL_VPDMA_LS_1;
            descParam[descCount].startY = chObj->coreFmt.startY;
        }
    }
    /* Set the other VPDMA parameters */
    if (FVID2_DF_YUV422I_YUYV == chObj->coreFmt.fmt.dataFormat)
    {
        descParam[VCORE_SEC_Y_CHANNEL_IDX].dataType =
            VPSHAL_VPDMA_CHANDT_YC422;
        descParam[VCORE_SEC_CBCR_CHANNEL_IDX].dataType =
            VPSHAL_VPDMA_CHANDT_YC422;
    }
    else if (FVID2_DF_YUV420SP_UV == chObj->coreFmt.fmt.dataFormat)
    {
        descParam[VCORE_SEC_Y_CHANNEL_IDX].dataType =
            VPSHAL_VPDMA_CHANDT_Y420;
        descParam[VCORE_SEC_CBCR_CHANNEL_IDX].dataType =
            VPSHAL_VPDMA_CHANDT_C420;
        /* If its the YUV420 data divided the CBCR channel frame height
        by half.*/
        descParam[VCORE_SEC_CBCR_CHANNEL_IDX].transHeight =
            descParam[VCORE_SEC_CBCR_CHANNEL_IDX].transHeight >> 1u;
        descParam[VCORE_SEC_CBCR_CHANNEL_IDX].frameHeight =
            descParam[VCORE_SEC_CBCR_CHANNEL_IDX].frameHeight >> 1u;
        descParam[VCORE_SEC_CBCR_CHANNEL_IDX].startY =
            descParam[VCORE_SEC_CBCR_CHANNEL_IDX].startY >> 1u;
    }
    else if (FVID2_DF_YUV422SP_UV == chObj->coreFmt.fmt.dataFormat)
    {
        descParam[VCORE_SEC_Y_CHANNEL_IDX].dataType =
            VPSHAL_VPDMA_CHANDT_Y422;
        descParam[VCORE_SEC_CBCR_CHANNEL_IDX].dataType =
            VPSHAL_VPDMA_CHANDT_C422;
    }
    else
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    for (descCount = 0; descCount < chObj->descInfo.numInDataDesc; descCount++)
    {
        descPtr =  descMem->inDataDesc[descCount];
        GT_assert(SecCoreTrace, (NULL != descPtr));
        retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr,
                &descParam[descCount]);
        if (VPS_SOK != retVal)
        {
            GT_0trace(SecCoreTrace, GT_ERR,
                "VPDMA Descriptor Creation Failed!!\n");
            break;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Make a dummy data descriptor at the last with zero transfer size
         * if required */
        descOffset = 0u;
        if (TRUE == chObj->isDummyNeeded)
        {
            for (descCount = 0u;
                 descCount < VCORE_SEC_NUM_DESC_PER_WINDOW;
                 descCount++)
            {
                descParam[descCount].transWidth = 0u;
                descParam[descCount].transHeight = 0u;
                descParam[descCount].startX =
                    descParam[descCount].frameWidth;
                descParam[descCount].startY =
                    descParam[descCount].frameHeight;
                descParam[descCount].channel =
                    hObj->parent->channels[descCount];
                descParam[descCount].nextChannel =
                    hObj->parent->channels[descCount];

                tempOffset = descCount;
                if (TRUE == swapLumaChroma)
                {
                    /* Program chroma descriptor first and then luma */
                    tempOffset =
                        (chObj->descInfo.numInDataDesc - 1u) - descCount;
                }

                /* Dummy descriptor is only descriptor in multiWinDataDesc */
                descPtr = descMem->multiWinDataDesc[tempOffset];
                GT_assert(SecCoreTrace, (NULL != descPtr));

                /* Form the descriptor */
                retVal = VpsHal_vpdmaCreateInBoundDataDesc(
                             descPtr,
                             &descParam[descCount]);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(SecCoreTrace, GT_ERR,
                        "VPDMA Descriptor Creation Failed!!\n");
                    break;
                }
                /* Set some address to make valid FID field */
                VpsHal_vpdmaSetAddress(descPtr, 0u, descPtr);
            }
            descOffset += VCORE_SEC_NUM_DESC_PER_WINDOW;
        }
        else if (VCORE_OPMODE_DISPLAY == hObj->parent->curMode)
        {
            /* Use dummy descriptor instead of dummy data descriptor */
            descPtr = descMem->multiWinDataDesc[0u];
            GT_assert(SecCoreTrace, (NULL != descPtr));

            for (cnt = 0u; cnt < (VCORE_SEC_NUM_DESC_PER_WINDOW * 2u); cnt++)
            {
                VpsHal_vpdmaCreateDummyDesc(descPtr);
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            descOffset += VCORE_SEC_NUM_DESC_PER_WINDOW;
        }

        if (VCORE_OPMODE_DISPLAY == hObj->parent->curMode)
        {
            /* Put SOCH on both luma and chroma first */
            descPtr = descMem->multiWinDataDesc[descOffset];
            GT_assert(SecCoreTrace, (NULL != descPtr));
            for (cnt = 0u; cnt < VCORE_SEC_NUM_DESC_PER_WINDOW; cnt++)
            {
#ifdef VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    descPtr,
                    hObj->parent->channels[cnt]);
#else
                VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            /* Increment offset every two ctrl desc as data desc is twice the
             * size of ctrl desc */
            descOffset += (VCORE_SEC_NUM_DESC_PER_WINDOW / 2u);

            /* Put Abort on both luma and chroma */
            descPtr = descMem->multiWinDataDesc[descOffset];
            GT_assert(SecCoreTrace, (NULL != descPtr));
            for (cnt = 0u; cnt < VCORE_SEC_NUM_DESC_PER_WINDOW; cnt++)
            {
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
                VpsHal_vpdmaCreateAbortCtrlDesc(
                    descPtr,
                    hObj->parent->channels[cnt]);
#else
                VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            /* Increment offset every two ctrl desc as data desc is twice the
             * size of ctrl desc */
            descOffset += (VCORE_SEC_NUM_DESC_PER_WINDOW / 2u);
        }
    }

    return (retVal);
}

static UInt32 secProgramReg(const Vcore_SecHandleObj *hObj,
                               Vcore_SecChObj *chObj,
                              const Vcore_DescMem *descMem)
{
    Int32               retVal = VPS_SOK;
    VpsHal_ChrusConfig  *chrusConfig;

    chrusConfig = &chObj->chrusConfig;

    if (FVID2_DF_YUV420SP_UV == chObj->coreFmt.fmt.dataFormat)
    {
        chrusConfig->cfgMode = VPSHAL_CHRUS_CFGMODE_A;
    }
    else if ((FVID2_DF_YUV422I_YUYV == chObj->coreFmt.fmt.dataFormat) ||
              FVID2_DF_YUV422SP_UV == chObj->coreFmt.fmt.dataFormat)
    {
        chrusConfig->cfgMode = VPSHAL_CHRUS_CFGMODE_B;
    }
    if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
    {
        chrusConfig->mode = VPS_SF_INTERLACED;
    }
    else
    {
        chrusConfig->mode = VPS_SF_PROGRESSIVE;
    }
    chrusConfig->coeff = NULL;

    if (VPS_SOK == retVal)
    {
        if (NULL != descMem)
        {
            retVal = VpsHal_chrusSetConfig(hObj->parent->chrusHandle,
                chrusConfig,
                descMem->shadowOvlyMem);
        }
        else
        {
            retVal = VpsHal_chrusSetConfig(hObj->parent->chrusHandle,
                chrusConfig,
                NULL);
        }
    }
    if (VPS_SOK == retVal && NULL != descMem &&
        NULL != descMem->nonShadowOvlyMem)
    {
        /* Overlay is only created.  Overlay will be set in update
         * buffer according to the line mode required */
        retVal = VpsHal_vpdmaCreateRegOverlay(hObj->parent->fsRegOffset,
            VCORE_SEC_NUM_VPDMA_FS_EVT_REG,
            descMem->nonShadowOvlyMem);
    }

    return (retVal);
}

static Int32 secUpdateRtParams(const Vcore_SecHandleObj *hObj,
                               Vcore_SecChObj *chObj,
                               const Vcore_SecRtParams *rtParams)
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
            coreFmt->memType = rtParams->inFrmPrms->memType;
            coreFmt->fmt.dataFormat = rtParams->inFrmPrms->dataFormat;
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
    /* Run time change of data format is supported only for
     * Memory to memory mode of operation
     */
    if (VCORE_OPMODE_MEMORY == hObj->parent->curMode)
    {
        if ((FVID2_DF_YUV420SP_UV == chObj->coreFmt.fmt.dataFormat) ||
                (FVID2_DF_YUV422SP_UV == chObj->coreFmt.fmt.dataFormat))
        {
            chObj->bufIndex[0] = FVID2_YUV_SP_Y_ADDR_IDX;
            chObj->bufIndex[1] = FVID2_YUV_SP_CBCR_ADDR_IDX;
        }
        else
        {
            chObj->bufIndex[0] = FVID2_YUV_INT_ADDR_IDX;
            chObj->bufIndex[1] = FVID2_YUV_INT_ADDR_IDX;
        }
    }

    /* Check whether the updated RT params are valid */
    retVal = secCheckFormat(hObj, coreFmt);
    if (VPS_SOK != retVal)
    {
        GT_0trace(SecCoreTrace, GT_ERR, "Invalid runtime parameters!!\n");
    }

    return (retVal);
}

static Int32 secUpdateRtDescMem(const Vcore_SecHandleObj *hObj,
                                Vcore_SecChObj *chObj,
                                const Vcore_DescMem *descMem)
{
    Int32     retVal;

    /* Re-program data descriptors */
    retVal = secProgramDataDesc(hObj, chObj, descMem);

    if (VPS_SOK == retVal)
    {
        /* Program the registers as input format can also change*/
        retVal = secProgramReg(hObj, chObj, descMem);
    }

    return (retVal);
}

/**
 *  secAllocHandleMem
 *  Allocate memory for handle objects from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_SecHandleObj *secAllocHandleMem(UInt32 numHandle)
{
    UInt32              instCnt;
    Vcore_SecHandleObj *hObjs = NULL;

    /* Check if the requested number of handles is within range */
    GT_assert(SecCoreTrace, (numHandle <= VCORE_SEC_MAX_HANDLES));

    Semaphore_pend(SecPoolSem, VCORE_SEC_SEM_TIMEOUT);

    for (instCnt = 0u; instCnt < VCORE_SEC_NUM_INST; instCnt++)
    {
        if (FALSE == SecHandleMemFlag[instCnt])
        {
            hObjs = &SecHandleMemPool[instCnt][0];
            SecHandleMemFlag[instCnt] = TRUE;
            break;
        }
    }

    if (NULL == hObjs)
    {
        GT_0trace(SecCoreTrace, GT_ERR, "Handle Object memory alloc failed!!\n");
    }
    Semaphore_post(SecPoolSem);

    return (hObjs);
}

/**
 *  secFreeHandleMem
 *  Frees-up the handle objects memory.
 */
static Int32 secFreeHandleMem(const Vcore_SecHandleObj *hObjs)
{
    UInt32              instCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != hObjs));

    Semaphore_pend(SecPoolSem, VCORE_SEC_SEM_TIMEOUT);

    for (instCnt = 0; instCnt < VCORE_SEC_NUM_INST; instCnt++)
    {
        if (&SecHandleMemPool[instCnt][0] == hObjs)
        {
            /* Check if the memory is already allocated */
            GT_assert(SecCoreTrace, (TRUE == SecHandleMemFlag[instCnt]));
            SecHandleMemFlag[instCnt] = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }
    Semaphore_post(SecPoolSem);

    return (retVal);
}

/**
 *  secAllocHandleObj
 *  Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
static Vcore_SecHandleObj *secAllocHandleObj(UInt32 instId,
                                        Vcore_OpMode mode)
{
    UInt32              instCnt, handleCnt;
    UInt32              cookie;
    Vcore_SecInstObj    *instObj = NULL;
    Vcore_SecHandleObj      *hObj = NULL;

    /* Find out the instance to which this handle belongs to. */
    for (instCnt = 0u; instCnt < SecProperty.numInstance; instCnt++)
    {
        if (SecInstObjects[instCnt].instId == instId)
        {
            instObj = &SecInstObjects[instCnt];
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
                GT_0trace(SecCoreTrace, GT_ERR,
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
 *  secAllocChannelMem
 *  Allocate memory for channel object from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_SecChObj *secAllocChannelMem(void)
{
    UInt32              channelCnt;
    Vcore_SecChObj *chObj = NULL;

    Semaphore_pend(SecPoolSem, VCORE_SEC_SEM_TIMEOUT);

    for (channelCnt = 0u; channelCnt < VCORE_SEC_MAX_CHANNEL_OBJECTS;
         channelCnt++)
    {
        if (FALSE == SecChannelMemPool[channelCnt].isAllocated)
        {
            chObj = &SecChannelMemPool[channelCnt];
            SecChannelMemPool[channelCnt].isAllocated = TRUE;
            break;
        }
    }
    if (NULL == chObj)
    {
        GT_0trace(SecCoreTrace, GT_ERR,
            "Channel Object memory alloc failed!!\n");
    }
    Semaphore_post(SecPoolSem);
    return (chObj);
}

/**
 *  secSetDefaultChInfo
 *  Resets the channel information with default values.
 */
static void secSetDefaultChInfo(Vcore_SecChObj *chObj,
                                UInt32 channelCnt)
{
    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isFormatSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /* Initialize format structure - Assumes YUV422 720x480 progressive */
    chObj->coreFmt.frameWidth = VCORE_SEC_DEFAULT_WIDTH;
    chObj->coreFmt.frameHeight = VCORE_SEC_DEFAULT_HEIGHT;
    chObj->coreFmt.startX = 0u;
    chObj->coreFmt.startY = 0u;
    chObj->coreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.memType = (UInt32) VPSHAL_VPDMA_MT_NONTILEDMEM;

    chObj->coreFmt.fmt.channelNum = channelCnt;
    chObj->coreFmt.fmt.width = VCORE_SEC_DEFAULT_WIDTH;
    chObj->coreFmt.fmt.height = VCORE_SEC_DEFAULT_HEIGHT;
    chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_SEC_DEFAULT_WIDTH * 2u;
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

    /* Initialize mosaic info structure */
    chObj->numExtraDesc = 0u;
    chObj->isDummyNeeded = FALSE;

    return;
}

/**
 *  secFreeChannelMem
 *  Frees-up the channel object memory.
 */
static Int32 secFreeChannelMem(const Vcore_SecChObj *chObj)
{
    UInt32              channelCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != chObj));

    Semaphore_pend(SecPoolSem, VCORE_SEC_SEM_TIMEOUT);

    for (channelCnt = 0; channelCnt < VCORE_SEC_MAX_CHANNEL_OBJECTS;
         channelCnt++)
    {
        if (&SecChannelMemPool[channelCnt] == chObj)
        {
            /* Check if the memory is already allocated */
            GT_assert(SecCoreTrace, (TRUE ==
                SecChannelMemPool[channelCnt].isAllocated));
            SecChannelMemPool[channelCnt].isAllocated = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }

    Semaphore_post(SecPoolSem);

    return (retVal);
}

/**
 *  secFreeChannelObj
 *  Frees-up the channel objects.
 */
static Int32 secFreeChannelObj(Vcore_SecChObj **chObjs,
                              UInt32 numCh)
{
    UInt32              channelCnt;
    Int32               retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != chObjs));

    for (channelCnt = 0; channelCnt < numCh; channelCnt++)
    {
        /* Memset the channel object before freeing */
        retVal += secFreeChannelMem(chObjs[channelCnt]);
        if (VPS_SOK == retVal)
        {
            VpsUtils_memset(chObjs[channelCnt], 0, sizeof(Vcore_SecChObj));
        }
        chObjs[channelCnt] = NULL;
    }

    return (retVal);
}

/**
 *  secAllocChannelObj
 *  Allocates channel objects.
 *  Returns error if allocation failed.
 */
static Int32 secAllocChannelObj(Vcore_SecChObj **chObjs,
                               UInt32 numCh)
{
    UInt32      channelCnt, errCnt;
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != chObjs));

    for (channelCnt = 0u; channelCnt < numCh; channelCnt++)
    {
        /* Allocate channel objects one at a time */
        chObjs[channelCnt] = secAllocChannelMem();
        if (NULL == chObjs[channelCnt])
        {
            GT_1trace(SecCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                channelCnt);
            retVal = VPS_EFAIL;
            break;
        }

        /* Set default values for the channel object */
        secSetDefaultChInfo(chObjs[channelCnt], channelCnt);
    }

    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        for (errCnt = 0u; errCnt < channelCnt; errCnt++)
        {
            secFreeChannelMem(chObjs[errCnt]);
            chObjs[errCnt] = NULL;
        }
    }

    return (retVal);
}

/**
 *  secFreeHandleObj
 *  Frees-up the handle object and resets the variables.
 */
static Int32 secFreeHandleObj(Vcore_SecHandleObj *hObj)
{
    UInt32              handleCnt;
    UInt32              cookie;
    Int32               retVal = VPS_EFAIL;
    Vcore_SecInstObj    *instObj;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(SecCoreTrace, (NULL != instObj));

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Free the handle object */
    for (handleCnt = 0u; handleCnt < instObj->numHandle; handleCnt++)
    {
        if (hObj == &instObj->hObjs[handleCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(SecCoreTrace, (TRUE == instObj->hObjs[handleCnt].used));

            /* Reset the variables */
            hObj->used = FALSE;
            hObj->parent = NULL;
            hObj->perChCfg = FALSE;
            hObj->numCh = 0u;
            secSetDefaultChInfo(&hObj->handleContext, 0u);

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
 *  secGetChObj
 *  Returns the channel object pointer depending on per channel config is
 *  enabled or not.
 */
static Vcore_SecChObj *secGetChObj(Vcore_SecHandleObj *hObj, UInt32 chNum)
{
    Vcore_SecChObj  *chObj;

    /* NULL pointer check */
    GT_assert(SecCoreTrace, (NULL != hObj));

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
