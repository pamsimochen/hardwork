/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiWrbkPath.c
 *
 *  \brief VPS DEI write back path core file.
 *  This file implements the core layer for DEI write back paths.
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
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_deiWrbkPath.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Number of descriptors needed per window. For this core only 422P
 *  out is supported. Hence it is always 1.
 */
#define VCORE_DWP_NUM_DESC_PER_WINDOW   (1u)

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_DWP_MAX_HANDLES * VCORE_DWP_MAX_CHANNELS
 *  objects per instance, only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_DWP_MAX_CHANNEL_OBJECTS   (VCORE_DWP_MAX_CHANNELS                \
                                       * VCORE_DWP_NUM_INST)

/** \brief Time out to be used in sem pend. */
#define VCORE_DWP_SEM_TIMEOUT           (BIOS_WAIT_FOREVER)

/** \brief Default width used for initializing format structure. */
#define VCORE_DWP_DEFAULT_WIDTH         (720u)
/** \brief Default height used for initializing format structure. */
#define VCORE_DWP_DEFAULT_HEIGHT        (480u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Typedef for DWP path instance object. */
typedef struct Vcore_DwpInstObj_t Vcore_DwpInstObj;

/**
 *  struct Vcore_DwpChObj
 *  \brief Per channel information.
 */
typedef struct
{
    Vcore_States                state;
    /**< Current state. */
    Vcore_Format                coreFmt;
    /**< Buffer formats and other formats. */
    Vcore_DescInfo              descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data and config descriptor memories required by this core. */
} Vcore_DwpChObj;

/**
 *  struct Vcore_DwpHandleObj
 *  \brief Per handle information.
 */
typedef struct
{
    UInt32                      isUsed;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_DwpInstObj           *instObj;
    /**< Pointer to the instance object. */
    UInt32                      perChCfg;
    /**< Flag indicating whether format is common for all the channels or
         it is different for each channel of a handle. */
    Vcore_DwpChObj             *chObjs[VCORE_DWP_MAX_CHANNELS];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool. */
    UInt32                      numCh;
    /**< Number of channels for this handle. This determines the number of
         valid pointers in channel array. */
} Vcore_DwpHandleObj;

/**
 *  struct Vcore_DwpInstObj_t
 *  \brief Structure containing per instance information.
 */
struct Vcore_DwpInstObj_t
{
    UInt32                      instId;
    /**< Instance number/ID. */
    UInt32                      numHandle;
    /**< Number of handles for this instance. This determines the size of
         handle array. */
    VpsHal_VpdmaChannel         vpdmaCh;
    /**< DEI-SC writeback VPDMA channel number required by core. */

    UInt32                      openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    Vcore_OpMode                curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or M2M operation of the core. */
    Vcore_DwpHandleObj          hObjs[VCORE_DWP_MAX_HANDLES];
    /**< Handle object pool. */
};

/**
 *  struct Vcore_DwpPoolObj
 *  Memory pool object containing all the statically allocated
 *  objects - used structure to avoid multiple global variables.
 */
typedef struct
{
    Vcore_DwpChObj              chMemPool[VCORE_DWP_MAX_CHANNEL_OBJECTS];
    /**< Memory pool for the channel objects. */
    UInt32                      chMemFlag[VCORE_DWP_MAX_CHANNEL_OBJECTS];
    /**< The flag variable represents whether a channel memory is allocated
         or not. */
    VpsUtils_PoolParams         chPoolPrm;
    /**< Pool params for descriptor pool memory. */

    Semaphore_Handle            lockSem;
    /**< Semaphore for protecting allocation and freeing of memory pool
         objects at open/close time. */
} Vcore_DwpPoolObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Vcore_Handle Vcore_dwpOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg);
static Int32 Vcore_dwpClose(Vcore_Handle handle);

static Int32 Vcore_dwpGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property);

static Int32 Vcore_dwpSetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_Format *coreFmt);
static Int32 Vcore_dwpGetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                Vcore_Format *coreFmt);

static Int32 Vcore_dwpGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo);

static Int32 Vcore_dwpSetFsEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem);

static Int32 Vcore_dwpProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem);
static Int32 Vcore_dwpProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem);

static Int32 Vcore_dwpUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid);
static Int32 Vcore_dwpUpdateRtMem(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  UInt32 chNum,
                                  const Void *rtPrms,
                                  UInt32 frmIdx);

static UInt32 vcoreDwpProgramDesc(const Vcore_DwpHandleObj *hObj,
                                  const Vcore_DwpChObj *chObj,
                                  const Vcore_DescMem *descMem);

static Int32 vcoreDwpCheckFmt(const Vcore_Format *coreFmt);

static Vcore_DwpHandleObj *vcoreDwpAllocHandleObj(UInt32 instId,
                                                  Vcore_OpMode mode);
static Int32 vcoreDwpFreeHandleObj(Vcore_DwpHandleObj *hObj);
static Int32 vcoreDwpAllocChObj(Vcore_DwpChObj **chObjs, UInt32 numCh);
static Int32 vcoreDwpFreeChObj(Vcore_DwpChObj **chObjs, UInt32 numCh);

static void vcoreDwpSetDefChInfo(Vcore_DwpChObj *chObj, UInt32 chNum);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief DWP path core function pointer. */
static const Vcore_Ops DwpCoreOps =
{
    Vcore_dwpGetProperty,           /* Get property */
    Vcore_dwpOpen,                  /* Open function */
    Vcore_dwpClose,                 /* Close function */
    Vcore_dwpSetFormat,             /* Set format */
    Vcore_dwpGetFormat,             /* Get format */
    NULL,                           /* Set mosaic */
    NULL,                           /* Create layout */
    NULL,                           /* Delete layout */
    Vcore_dwpGetDescInfo,           /* Get descriptor info */
    Vcore_dwpSetFsEvent,            /* Set frame */
    NULL,                           /* Set parameters */
    NULL,                           /* Get parameters */
    NULL,                           /* Control */
    Vcore_dwpProgramDesc,           /* Program descriptor */
    Vcore_dwpProgramReg,            /* Program register */
    Vcore_dwpUpdateDesc,            /* Update descriptor */
    NULL,                           /* Update multiple window descriptor */
    NULL,                           /* Get DEI context info */
    NULL,                           /* Update context descriptor */
    Vcore_dwpUpdateRtMem,           /* Update runtime params */
    NULL                            /* Program scalar coefficient */
};

/** \brief Properties of this core. */
static Vcore_Property DwpProperty;

/** \brief DWP path objects. */
static Vcore_DwpInstObj DwpInstObjects[VCORE_DWP_NUM_INST];

/**
 *  \brief Pool objects used for storing pool memories, pool flags and pool
 *  handles.
 */
static Vcore_DwpPoolObj VcoreDwpPoolObjs;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_dwpInit
 *  \brief DWP core init function.
 *  Initializes DWP core objects, allocates memory etc.
 *  This function should be called before calling any of DWP core API's.
 *
 *  \param numInst      [IN] Number of instance objects to be initialized.
 *  \param initPrms     [IN] Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_dwpInit(UInt32 numInst,
                    const Vcore_DwpInitParams *initPrms,
                    Ptr arg)
{
    Int32               retVal = VPS_SOK;
    UInt32              instCnt, hCnt;
    Vcore_DwpInstObj   *instObj;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(DwpCoreTrace, (numInst <= VCORE_DWP_NUM_INST));
    GT_assert(DwpCoreTrace, (NULL != initPrms));

    /* Init pool objects */
    VpsUtils_initPool(
        &VcoreDwpPoolObjs.chPoolPrm,
        (Void *) VcoreDwpPoolObjs.chMemPool,
        VCORE_DWP_MAX_CHANNEL_OBJECTS,
        sizeof (Vcore_DwpChObj),
        VcoreDwpPoolObjs.chMemFlag,
        DwpCoreTrace);

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    VcoreDwpPoolObjs.lockSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == VcoreDwpPoolObjs.lockSem)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        for (instCnt = 0u; instCnt < numInst; instCnt++)
        {
            /* Initialize instance object members */
            instObj = &DwpInstObjects[instCnt];
            instObj->instId = initPrms[instCnt].instId;
            instObj->vpdmaCh = initPrms[instCnt].vpdmaCh;
            instObj->openCnt = 0u;
            instObj->curMode = VCORE_OPMODE_INVALID;
            instObj->numHandle = initPrms[instCnt].maxHandle;

            /* Mark all handles as free */
            for (hCnt = 0u; hCnt < VCORE_DWP_MAX_HANDLES; hCnt++)
            {
                instObj->hObjs[hCnt].isUsed = FALSE;
            }
        }

        /* Initialize DWP path core properties */
        DwpProperty.numInstance = numInst;
        DwpProperty.name = VCORE_TYPE_DEI_WB;
        DwpProperty.type = VCORE_TYPE_OUTPUT;
        DwpProperty.internalContext = FALSE;
    }

    return (retVal);
}



/**
 *  Vcore_dwpDeInit
 *  \brief DWP core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_dwpDeInit(Ptr arg)
{
    UInt32              instCnt;
    Vcore_DwpInstObj   *instObj;

    /* Free-up handle objects for each instance if already allocated */
    for (instCnt = 0u; instCnt < VCORE_DWP_NUM_INST; instCnt++)
    {
        instObj = &DwpInstObjects[instCnt];
        instObj->instId = VCORE_DWP_INST_WB0;
        instObj->curMode = VCORE_OPMODE_INVALID;
        instObj->numHandle = 0u;
    }

    /* Delete the Semaphore created for pool objects */
    if (NULL != VcoreDwpPoolObjs.lockSem)
    {
        Semaphore_delete(&VcoreDwpPoolObjs.lockSem);
        VcoreDwpPoolObjs.lockSem = NULL;
    }

    DwpProperty.numInstance = 0u;

    return (VPS_SOK);
}



/**
 *  Vcore_dwpGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_dwpGetCoreOps(void)
{
    return &DwpCoreOps;
}



/**
 *  Vcore_dwpOpen
 *  \brief Opens a particular instance of DWP path in either capture
 *  or in M2M mode and returns the handle to the handle object.
 *  When opened in capture mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in M2M mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_DWP_MAX_HANDLES macro. Also each of the handles could have multiple
 *  channel to support more than one M2M operation is a single request.
 *
 *  \param instId       [IN] Instance to open - DEI HQ or DEI instance.
 *  \param mode         [IN] Mode to open the instance for - capture or M2M.
 *  \param numCh        [IN] Number of channel to be associated with this open.
 *                      For capture operation only one channel is allowed.
 *                      For M2M operation, more than one channel could
 *                      be associated with a handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_DWP_MAX_CHANNELS macro.
 *  \param perChCfg     [IN] Flag indicating whether the config descriptors
 *                      should be separate for each and every channel or
 *                      not. This is valid only for M2M drivers. For
 *                      capture drivers this should be set to FALSE.
 *                      TRUE - Separate config descriptors for each channel.
 *                      FALSE - Same configuration for all the channels.
 *
 *  \return             If success returns the handle else returns NULL.
 */
static Vcore_Handle Vcore_dwpOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg)
{
    Int32               retVal = VPS_SOK;
    Vcore_DwpHandleObj *hObj = NULL;

    /* Only capture and memory modes are supported */
    GT_assert(DwpCoreTrace,
        ((VCORE_OPMODE_CAPTURE == mode) || (VCORE_OPMODE_MEMORY == mode)));
    /* Check for maximum channel supported per handle */
    GT_assert(DwpCoreTrace, (numCh <= VCORE_DWP_MAX_CHANNELS));
    /* There should be atleast one channel */
    GT_assert(DwpCoreTrace, (0u != numCh));
    /* For capture operation, only one channel should be allowed */
    GT_assert(DwpCoreTrace,
        (!((VCORE_OPMODE_CAPTURE == mode) && (VCORE_MAX_CAPTURE_CH < numCh))));
    /* For capture operation, per channel format should be FALSE */
    GT_assert(DwpCoreTrace,
        (!((VCORE_OPMODE_CAPTURE == mode) && (FALSE != perChCfg))));

    /* Allocate a handle object */
    hObj = vcoreDwpAllocHandleObj(instId, mode);
    if (NULL != hObj)
    {
        /* Allocate channel objects */
        retVal = vcoreDwpAllocChObj(hObj->chObjs, numCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DwpCoreTrace, GT_ERR, "Channel allocation failed!!\n");
            /* Free-up handle object if channel allocation failed */
            vcoreDwpFreeHandleObj(hObj);
            hObj = NULL;
        }
        else
        {
            /* Initialize variables */
            hObj->perChCfg = perChCfg;
            hObj->numCh = numCh;
        }
    }
    else
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Handle allocation failed!!\n");
    }

    return ((Vcore_Handle) hObj);
}



/**
 *  Vcore_dwpClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpClose(Vcore_Handle handle)
{
    Int32                   retVal = VPS_EFAIL;
    Vcore_DwpHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    hObj = (Vcore_DwpHandleObj *) handle;

    /* Free channel and handle objects */
    retVal = vcoreDwpFreeChObj(hObj->chObjs, hObj->numCh);
    retVal |= vcoreDwpFreeHandleObj(hObj);
    if (VPS_SOK != retVal)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Free memory failed!!\n");
    }

    return (retVal);
}



/**
 *  Vcore_dwpGetProperty
 *  \brief Gets the core properties of the DWP path core.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param property     [OUT] Pointer to which the DWP path core properties
 *                      to be copied.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property)
{
    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != property));

    VpsUtils_memcpy(property, &DwpProperty, sizeof(Vcore_Property));

    return (VPS_SOK);
}



/**
 *  Vcore_dwpSetFormat
 *  \brief Sets the format for a given channel.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel to which the params should be applied.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param coreFmt      [IN] Pointer to the format information. This should
 *                      point to a valid Vcore_Format structure.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpSetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_Format *coreFmt)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt, startCh, numCh;
    Vcore_DwpChObj         *chObj;
    Vcore_DwpHandleObj     *hObj;
    Vcore_DwpInstObj       *instObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != coreFmt));
    hObj = (Vcore_DwpHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DwpCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the requested format is valid */
        retVal = vcoreDwpCheckFmt(coreFmt);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DwpCoreTrace, GT_ERR, "Invalid Format!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE == hObj->perChCfg)
        {
            /* If configuration is per channel, then set the format only for
             * that channel */
            startCh = chNum;
            numCh = 1u;
        }
        else
        {
            /* If configuration is per handle, then set the format for all
             * the channels in the handle */
            startCh = 0u;
            numCh = hObj->numCh;
        }

        for (chCnt = startCh; chCnt < (startCh + numCh); chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            /* NULL pointer check */
            GT_assert(DwpCoreTrace, (NULL != chObj));

            /* Copy the format to the local structure */
            VpsUtils_memcpy(&chObj->coreFmt, coreFmt, sizeof(Vcore_Format));

            /*
             *  Set the channel information depending on DWPs requirement.
             *  Only one out descriptor is required.
             */
            chObj->descInfo.numOutDataDesc = VCORE_DWP_NUM_DESC_PER_WINDOW;
            chObj->descInfo.socChNum[0u] = instObj->vpdmaCh;
            chObj->descInfo.numChannels = VCORE_DWP_NUM_DESC_PER_WINDOW;

            /* Set proper state - format is set */
            chObj->state.isFormatSet = TRUE;
        }
    }

    return (retVal);
}



/**
 *  Vcore_dwpGetFormat
 *  \brief Gets the format for a given channel.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel from which the params should be read.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param coreFmt      [OUT] Pointer to the copied format information. This
 *                      should point to a valid Vcore_Format structure.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpGetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                Vcore_Format *coreFmt)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DwpChObj         *chObj;
    Vcore_DwpHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != coreFmt));
    hObj = (Vcore_DwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DwpCoreTrace, (NULL != chObj));

        /* Copy the format */
        VpsUtils_memcpy(coreFmt, &chObj->coreFmt, sizeof(Vcore_Format));
    }

    return (retVal);
}



/**
 *  Vcore_dwpGetDescInfo
 *  \brief Depending on the format set, returns the number of data
 *  descriptors and config overlay memory needed for the actual driver to
 *  allocate memory.
 *  Format should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel to which the info is required.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param layoutId     [IN] Layout ID for which to return the information.
 *                      Not used in this core, set this to
 *                      VCORE_DEFAULT_LAYOUT_ID.
 *  \param descInfo     [OUT] Pointer to the number of data descriptor and
 *                      config overlay memory size information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DwpChObj         *chObj;
    Vcore_DwpHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != descInfo));
    hObj = (Vcore_DwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DwpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(DwpCoreTrace, GT_ERR,
                "Set the format before getting the descriptor info!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Copy the data descriptor information */
        VpsUtils_memcpy(
            (Ptr) descInfo,
            &chObj->descInfo,
            sizeof(Vcore_DescInfo));
    }

    return (retVal);
}



/**
 *  Vcore_dwpSetFsEvent
 *  \brief Program the client's frame start.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param fsEvent      [IN] Frame start event.
 *  \param descMem      [IN] Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actual registers. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *                      Note: In the current implementation only direct register
 *                      write is performed as the line mode is always the same.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpSetFsEvent(Vcore_Handle handle,
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
 *  Vcore_dwpProgramDesc
 *  \brief Programs the data descriptor for a given channel.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for M2M operation.
 *  \param layoutId     [IN] Layout ID used to program the descriptor.
 *                      Not used in this core, set this to
 *                      VCORE_DEFAULT_LAYOUT_ID.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DwpChObj         *chObj;
    Vcore_DwpHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != descMem));
    hObj = (Vcore_DwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DwpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(DwpCoreTrace, GT_ERR,
                "Set the format before programming the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        retVal = vcoreDwpProgramDesc(hObj, chObj, descMem);
    }

    return (retVal);
}



/**
 *  Vcore_dwpProgramReg
 *  \brief Programs the registers for each of the HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in capture mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for M2M operation.
 *  \param descMem      [IN] Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actual registers. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem)
{
    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));

    /* Core does not support any register programming. Always return sucecss */
    return (VPS_SOK);
}



/**
 *  Vcore_dwpUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frame        [IN] Pointer to the FVID frame containing the buffer
 *                      address. This parameter should be non-NULL.
 *  \param fid          [IN] FID to be programmed in the descriptor.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chNum;
    Vcore_DwpChObj         *chObj;
    Vcore_DwpHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != descMem));
    GT_assert(DwpCoreTrace, (NULL != frame));
    hObj = (Vcore_DwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    chNum = frame->channelNum;
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DwpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(DwpCoreTrace, GT_ERR,
                "Set the format before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Update data descriptor */
        GT_assert(DwpCoreTrace, (NULL != descMem->outDataDesc[0u]));
        VpsHal_vpdmaSetAddress(
            descMem->outDataDesc[0u],
            0u,                 /* FID is reserved for outbound descriptors */
            frame->addr[fid][FVID2_YUV_INT_ADDR_IDX]);

        /* For outbound descriptors if frame address is NULL,
         * drop the data - don't return error */
        if (NULL == frame->addr[fid][FVID2_YUV_INT_ADDR_IDX])
        {
            VpsHal_vpdmaSetDropData(descMem->outDataDesc[0u], TRUE);

            /* Workaround: Drop data and Mode bit can not both be set.
             * So clear memType when setting drop data */
            VpsHal_vpdmaSetMemType(
                descMem->outDataDesc[0u],
                VPSHAL_VPDMA_MT_NONTILEDMEM);
        }
        else
        {
            VpsHal_vpdmaSetDropData(descMem->outDataDesc[0u], FALSE);

            /* Set memory type back to original because of above
             * workaround */
            VpsHal_vpdmaSetMemType(
                descMem->outDataDesc[0u],
                (VpsHal_VpdmaMemoryType) chObj->coreFmt.memType);
        }
    }

    return (retVal);
}



/**
 *  Vcore_dwpUpdateRtMem
 *  \brief Updates the DWP runtime parameters in the descriptor as well as
 *  in register overlay memories.
 *
 *  \param handle       [IN] DEI Writeback Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for M2M operation.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param rtPrms       [IN] Pointer to the Vcore_DwpRtParams structure
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
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_dwpUpdateRtMem(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  UInt32 chNum,
                                  const Void *rtPrms,
                                  UInt32 frmIdx)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      cnt;
    UInt32                      chCnt, startCh, numCh;
    Void                       *descPtr;
    Vcore_DwpChObj             *chObj = NULL;
    Vcore_DwpHandleObj         *hObj;
    Vcore_Format               *coreFmt;
    const Vcore_DwpRtParams    *dwpRtPrms;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != handle));
    GT_assert(DwpCoreTrace, (NULL != descMem));
    hObj = (Vcore_DwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE == hObj->perChCfg)
        {
            /* If configuration is per channel, then set the runtime changes
             * only for that channel */
            startCh = chNum;
            numCh = 1u;
        }
        else
        {
            /* If configuration is per handle, then set the runtime changes
             * for all the channels in the handle */
            startCh = 0u;
            numCh = hObj->numCh;
        }

        for (chCnt = startCh; chCnt < (startCh + numCh); chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            /* NULL pointer check */
            GT_assert(DwpCoreTrace, (NULL != chObj));

            if (TRUE != chObj->state.isFormatSet)
            {
                GT_0trace(DwpCoreTrace, GT_ERR,
                    "Set the parameter before updating the descriptors!!\n");
                retVal = VPS_EFAIL;
            }

            /* First update the configuration if present */
            coreFmt = &chObj->coreFmt;
            if (NULL != rtPrms)
            {
                dwpRtPrms = (const Vcore_DwpRtParams *) rtPrms;

                /* Copy the out frame params to core format */
                if (NULL != dwpRtPrms->outFrmPrms)
                {
                    coreFmt->fmt.width = dwpRtPrms->outFrmPrms->width;
                    coreFmt->fmt.height = dwpRtPrms->outFrmPrms->height;
                    coreFmt->frameWidth = coreFmt->fmt.width;
                    coreFmt->frameHeight = coreFmt->fmt.height;
                    for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                    {
                        coreFmt->fmt.pitch[cnt] =
                            dwpRtPrms->outFrmPrms->pitch[cnt];
                    }
                    coreFmt->memType = dwpRtPrms->outFrmPrms->memType;
                }
            }
        }
    }

    /* Update out frame parameters in the descriptor descriptors. */
    if (VPS_SOK == retVal)
    {
        descPtr = descMem->outDataDesc[0u];
        GT_assert(DwpCoreTrace, (NULL != descPtr));

        /* Update pitch and memory type in VPDMA descriptor memory */
        GT_assert(DwpCoreTrace, (NULL != chObj));
        VpsHal_vpdmaSetLineStride(
            descPtr,
            chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX]);
        VpsHal_vpdmaSetMemType(
            descPtr,
            (VpsHal_VpdmaMemoryType) chObj->coreFmt.memType);
    }

    return (retVal);
}



/**
 *  vcoreDwpProgramDesc
 *  \brief Programs the data descriptor depending on the format set.
 *  This is used in case of non-mosaic mode of operation.
 */
static UInt32 vcoreDwpProgramDesc(const Vcore_DwpHandleObj *hObj,
                                  const Vcore_DwpChObj *chObj,
                                  const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    Vcore_DwpInstObj           *instObj;
    VpsHal_VpdmaOutDescParams   outDescPrm;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != hObj));
    GT_assert(DwpCoreTrace, (NULL != chObj));
    GT_assert(DwpCoreTrace, (NULL != descMem));
    instObj = hObj->instObj;
    GT_assert(DwpCoreTrace, (NULL != instObj));

    outDescPrm.lineStride =
        chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX];
    outDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    outDescPrm.notify = FALSE;
    if (VCORE_OPMODE_CAPTURE == instObj->curMode)
    {
        outDescPrm.priority = VPS_CFG_CAPT_WB_VPDMA_PRIORITY;
    }
    else
    {
        outDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR;
    }
    outDescPrm.memType = (VpsHal_VpdmaMemoryType) chObj->coreFmt.memType;
    outDescPrm.maxWidth = VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
    outDescPrm.maxHeight = VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED;
    outDescPrm.is1DMem = FALSE;
    outDescPrm.dataType = VPSHAL_VPDMA_CHANDT_YC422;
    outDescPrm.channel = instObj->vpdmaCh;
    outDescPrm.nextChannel = instObj->vpdmaCh;
    if ((VCORE_OPMODE_CAPTURE == instObj->curMode) &&
        (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat) &&
        (TRUE == chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX]))
    {
        outDescPrm.lineSkip = VPSHAL_VPDMA_LS_2;
    }
    else
    {
        outDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    }

    /* Program descriptor */
    GT_assert(DwpCoreTrace, (NULL != descMem->outDataDesc[0u]));
    retVal = VpsHal_vpdmaCreateOutBoundDataDesc(
                 descMem->outDataDesc[0u],
                 &outDescPrm);
    if (VPS_SOK != retVal)
    {
        GT_0trace(DwpCoreTrace, GT_ERR, "VPDMA Descriptor Creation Failed!!\n");
    }
    else
    {
        /* Print the created descriptor memory */
        GT_0trace(DwpCoreTrace, GT_DEBUG, "Out Data Descrirptor:\n");
        GT_0trace(DwpCoreTrace, GT_DEBUG, "---------------------\n");
        VpsHal_vpdmaPrintDesc(descMem->outDataDesc[0u], DwpCoreTrace);
    }

    return (retVal);
}



/**
 *  vcoreDwpCheckFmt
 *  \brief Checks whether a given format if valid or not.
 *  Returns VPS_SOK if everything is fine else returns error value.
 */
static Int32 vcoreDwpCheckFmt(const Vcore_Format *coreFmt)
{
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != coreFmt));

    /* Check whether data format is supported or not */
    if (!(FVID2_DF_YUV422I_YUYV == coreFmt->fmt.dataFormat))
    {
        GT_1trace(DwpCoreTrace, GT_ERR,
            "Data format (%d) not supported!!\n", coreFmt->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_DF_YUV422I_YUYV == coreFmt->fmt.dataFormat)
    {
        /* Check whether the pitch is valid */
        if (coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX]
            < (coreFmt->fmt.width * 2u))
        {
            GT_2trace(DwpCoreTrace, GT_ERR,
                "Pitch (%d) less than Width (%d) in bytes!!\n",
                coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX],
                (coreFmt->fmt.width * 2u));
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    /* Check whether window width is even */
    if (coreFmt->fmt.width & 0x01u)
    {
        GT_1trace(DwpCoreTrace, GT_ERR,
            "Width(%d) can't be odd!!\n", coreFmt->fmt.width);
        retVal = VPS_EINVALID_PARAMS;
    }

    return (retVal);
}



/**
 *  vcoreDwpAllocHandleObj
 *  \brief Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
static Vcore_DwpHandleObj *vcoreDwpAllocHandleObj(UInt32 instId,
                                                  Vcore_OpMode mode)
{
    UInt32              instCnt, hCnt;
    Vcore_DwpInstObj   *instObj = NULL;
    Vcore_DwpHandleObj *hObj = NULL;

    /* Find out the instance to which this handle belongs to */
    for (instCnt = 0u; instCnt < VCORE_DWP_NUM_INST; instCnt++)
    {
        if (DwpInstObjects[instCnt].instId == instId)
        {
            instObj = &DwpInstObjects[instCnt];
            break;
        }
    }

    Semaphore_pend(VcoreDwpPoolObjs.lockSem, VCORE_DWP_SEM_TIMEOUT);

    if (NULL != instObj)
    {
        /* Check if the required mode is proper */
        if (VCORE_OPMODE_INVALID > instObj->curMode)
        {
            /* Core already opend for some mode. For capture mode, only one
             * handle should be allowed. It doesn't make sense isn't it? */
            GT_assert(DwpCoreTrace, (VCORE_OPMODE_CAPTURE != instObj->curMode));
            GT_assert(DwpCoreTrace,
                !((VCORE_OPMODE_MEMORY == instObj->curMode)
                    && (VCORE_OPMODE_MEMORY != mode)));
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
        /* Update the state and return the handle */
        hObj->isUsed = TRUE;
        hObj->instObj = instObj;
        instObj->openCnt++;
        instObj->curMode = mode;
    }

    Semaphore_post(VcoreDwpPoolObjs.lockSem);

    return (hObj);
}



/**
 *  vcoreDwpFreeHandleObj
 *  \brief Frees-up the handle object and resets the variables.
 */
static Int32 vcoreDwpFreeHandleObj(Vcore_DwpHandleObj *hObj)
{
    UInt32              hCnt;
    Int32               retVal = VPS_EFAIL;
    Vcore_DwpInstObj   *instObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(DwpCoreTrace, (NULL != instObj));

    Semaphore_pend(VcoreDwpPoolObjs.lockSem, VCORE_DWP_SEM_TIMEOUT);

    /* Free the handle object */
    for (hCnt = 0u; hCnt < instObj->numHandle; hCnt++)
    {
        if (hObj == &instObj->hObjs[hCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(DwpCoreTrace, (TRUE == instObj->hObjs[hCnt].isUsed));

            /* Reset the variables */
            hObj->isUsed = FALSE;
            hObj->instObj = NULL;
            hObj->perChCfg = FALSE;
            hObj->numCh = 0u;

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

    Semaphore_post(VcoreDwpPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreDwpAllocChObj
 *  \brief Allocates channel objects.
 *  Returns error if allocation failed.
 */
static Int32 vcoreDwpAllocChObj(Vcore_DwpChObj **chObjs, UInt32 numCh)
{
    Int32       retVal = VPS_SOK;
    UInt32      chCnt;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != chObjs));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Allocate channel objects one at a time */
        chObjs[chCnt] = (Vcore_DwpChObj *)
            VpsUtils_alloc(
                &VcoreDwpPoolObjs.chPoolPrm,
                sizeof (Vcore_DwpChObj),
                DwpCoreTrace);
        if (NULL == chObjs[chCnt])
        {
            GT_1trace(DwpCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                chCnt);
            retVal = VPS_EALLOC;
            break;
        }

        /* Set default values for the channel object */
        vcoreDwpSetDefChInfo(chObjs[chCnt], chCnt);
    }

    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        vcoreDwpFreeChObj(chObjs, chCnt);
    }

    return (retVal);
}



/**
 *  vcoreDwpFreeChObj
 *  \brief Frees-up the channel objects.
 */
static Int32 vcoreDwpFreeChObj(Vcore_DwpChObj **chObjs, UInt32 numCh)
{
    UInt32          chCnt;
    Int32           retVal = VPS_SOK;
    Vcore_DwpChObj *chObj;

    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != chObjs));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        chObj = chObjs[chCnt];
        GT_assert(DwpCoreTrace, (NULL != chObj));

        /* Memset the channel object before freeing */
        VpsUtils_memset(chObj, 0u, sizeof(Vcore_DwpChObj));
        retVal |= VpsUtils_free(
                      &VcoreDwpPoolObjs.chPoolPrm,
                      chObj,
                      DwpCoreTrace);
        chObjs[chCnt] = NULL;
    }

    return (retVal);
}



/**
 *  vcoreDwpSetDefChInfo
 *  \brief Resets the channel information with default values.
 */
static void vcoreDwpSetDefChInfo(Vcore_DwpChObj *chObj, UInt32 chNum)
{
    /* NULL pointer check */
    GT_assert(DwpCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isFormatSet = FALSE;
    chObj->state.isParamsSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /* Initialize core format - Assumes YUV422 720x480 progressive */
    chObj->coreFmt.frameWidth = VCORE_DWP_DEFAULT_WIDTH;
    chObj->coreFmt.frameHeight = VCORE_DWP_DEFAULT_HEIGHT;
    chObj->coreFmt.startX = 0u;
    chObj->coreFmt.startY = 0u;
    chObj->coreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.memType = (UInt32) VPSHAL_VPDMA_MT_NONTILEDMEM;

    chObj->coreFmt.fmt.channelNum = chNum;
    chObj->coreFmt.fmt.width = VCORE_DWP_DEFAULT_WIDTH;
    chObj->coreFmt.fmt.height = VCORE_DWP_DEFAULT_HEIGHT;
    chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_DWP_DEFAULT_WIDTH * 2u;
    chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chObj->coreFmt.fmt.dataFormat = FVID2_DF_YUV422I_YUYV;
    chObj->coreFmt.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.fmt.bpp = FVID2_BPP_BITS16;
    chObj->coreFmt.fmt.reserved = NULL;

    /* Initialize channel descriptor info structure */
    VpsUtils_memset(&chObj->descInfo, 0u, sizeof(Vcore_DescInfo));

    return;
}
