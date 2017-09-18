/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiApi.c
 *
 *  \brief VPS DEI Path Core file.
 *  This file implements the core layer for DEI path containing
 *  VC Range mapping, CHRUS, DRN, DEIHQ/DEI and SC_H/SC.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/core/src/vpscore_deiPriv.h>
#include <ti/psp/vps/vps_advCfgDeiHq.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Vcore_Handle Vcore_deiOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg);
static Int32 Vcore_deiClose(Vcore_Handle handle);

static Int32 Vcore_deiGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property);

static Int32 Vcore_deiSetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                const Void *prms);
static Int32 Vcore_deiGetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                Void *prms);

static Int32 Vcore_deiCreateLayout(Vcore_Handle handle,
                                   UInt32 chNum,
                                   const Vps_MultiWinParams *multiWinPrms,
                                   UInt32 *layoutId);
static Int32 Vcore_deiDeleteLayout(Vcore_Handle handle,
                                   UInt32 chNum,
                                   UInt32 layoutId);

static Int32 Vcore_deiGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo);
static Int32 Vcore_deiGetDeiCtxInfo(Vcore_Handle handle,
                                    UInt32 chNum,
                                    Vcore_DeiCtxInfo *ctxInfo);
static Int32 Vcore_deiSetFsEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem);

static Int32 Vcore_deiControl(Vcore_Handle handle,
                              UInt32 cmd,
                              Ptr args);

static Int32 Vcore_deiProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem);
static Int32 Vcore_deiProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem);

static Int32 Vcore_deiUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid);
static Int32 Vcore_deiUpdateMultiDesc(Vcore_Handle handle,
                                      UInt32 layoutId,
                                      const Vcore_DescMem *descMem,
                                      const FVID2_FrameList *frameList,
                                      UInt32 fid,
                                      UInt32 startIdx,
                                      UInt32 *frmConsumed);

static Int32 Vcore_deiUpdateContext(Vcore_Handle handle,
                                    UInt32 chNum,
                                    const Vcore_DescMem *descMem,
                                    const Void *ctx);
static Int32 Vcore_deiUpdateRtMem(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  UInt32 chNum,
                                  const Void *rtPrms,
                                  UInt32 frmIdx);
static Int32 Vcore_deiProgramScCoeff(Vcore_Handle handle,
                                     UInt32 chNum,
                                     Vcore_ScCoeffParams *coeffPrms);
static Int32 Vcore_deiUpdateMode(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem,
                                 UInt32 prevFldState);
static Int32 Vcore_deiGetScFactorConfig(Vcore_Handle handle,
                                        UInt32 chNum,
                                        VpsHal_ScFactorConfig *scFactorConfig,
                                        const Vcore_DescMem *descMem);

static Int32 vcoreDeiUpdateRtPrms(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj,
                                  const Vcore_DeiRtParams *rtPrms);
static Int32 vcoreDeiUpdateRtDescMem(const Vcore_DeiInstObj *instObj,
                                     Vcore_DeiChObj *chObj,
                                     const Vcore_DescMem *descMem);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief DEI path core function pointer. */
static const Vcore_Ops DeiCoreOps =
{
    Vcore_deiGetProperty,           /* Get property */
    Vcore_deiOpen,                  /* Open function */
    Vcore_deiClose,                 /* Close function */
    NULL,                           /* Set format */
    NULL,                           /* Get format */
    NULL,                           /* Set mosaic */
    Vcore_deiCreateLayout,          /* Create layout */
    Vcore_deiDeleteLayout,          /* Delete layout */
    Vcore_deiGetDescInfo,           /* Get descriptor info */
    Vcore_deiSetFsEvent,            /* Set frame */
    Vcore_deiSetParams,             /* Set parameters */
    Vcore_deiGetParams,             /* Get parameters */
    Vcore_deiControl,               /* Control */
    Vcore_deiProgramDesc,           /* Program descriptor */
    Vcore_deiProgramReg,            /* Program register */
    Vcore_deiUpdateDesc,            /* Update descriptor */
    Vcore_deiUpdateMultiDesc,       /* Update multiple window descriptor */
    Vcore_deiGetDeiCtxInfo,         /* Get DEI context info */
    Vcore_deiUpdateContext,         /* Update context descriptor */
    Vcore_deiUpdateRtMem,           /* Update runtime params */
    Vcore_deiProgramScCoeff,        /* Program scalar coefficient */
    Vcore_deiUpdateMode,            /* Update the mode in DEI */
    Vcore_deiGetScFactorConfig      /* Get scaling factor config */
};

/** \brief Properties of this core. */
static Vcore_Property VcoreDeiProperty;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_deiInit
 *  \brief DEI core init function.
 *  Initializes DEI core objects, allocates memory etc.
 *  This function should be called before calling any of DEI core API's.
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
Int32 Vcore_deiInit(UInt32 numInst,
                    const Vcore_DeiInitParams *initPrms,
                    Ptr arg)
{
    Int32               retVal = VPS_SOK;

    /* Check for errors */
    GT_assert(DeiCoreTrace, (numInst <= VCORE_DEI_INST_MAX));
    GT_assert(DeiCoreTrace, (NULL != initPrms));

    /* Initialize the internal objects */
    retVal = vcoreDeiInit(numInst, initPrms);
    if (VPS_SOK == retVal)
    {
        /* Initialize DEI path core properties */
        VcoreDeiProperty.numInstance = numInst;
        VcoreDeiProperty.name = VCORE_TYPE_DEI;
        VcoreDeiProperty.type = VCORE_TYPE_INPUT;
        VcoreDeiProperty.internalContext = TRUE;
    }

    /* Cleanup if error occurs */
    if (VPS_SOK != retVal)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Init Failed!!\n");
        Vcore_deiDeInit(NULL);
    }

    return (retVal);
}



/**
 *  Vcore_deiDeInit
 *  \brief DEI core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_deiDeInit(Ptr arg)
{
    Int32       retVal = VPS_SOK;

    retVal = vcoreDeiDeInit();
    VcoreDeiProperty.numInstance = 0u;

    return (retVal);
}



/**
 *  Vcore_deiGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_deiGetCoreOps(void)
{
    return &DeiCoreOps;
}



/**
 *  Vcore_deiOpen
 *  \brief Opens a particular instance of DEI path in either display
 *  or in M2M mode and returns the handle to the handle object.
 *  When opened in display mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in M2M mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_DEI_MAX_HANDLES macro. Also each of the handles could have multiple
 *  channel to support more than one M2M operation is a single request.
 *
 *  \param instId       [IN] Instance to open.
 *  \param mode         [IN] Mode to open the instance for - display or M2M.
 *  \param numCh        [IN] Number of channel to be associated with this open.
 *                      For display operation only one channel is allowed.
 *                      For M2M operation, more than one channel could
 *                      be associated with a handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_DEI_MAX_CHANNELS macro.
 *  \param perChCfg     [IN] Flag indicating whether the channel configuration
 *                      should be separate for each and every channel or
 *                      not. This is valid only for M2M drivers. For
 *                      display drivers this should be set to FALSE.
 *                      TRUE - Separate config descriptors for each channel.
 *                      FALSE - Same configuration for all the channels.
 *
 *  \return             If success returns the handle else returns NULL.
 */
static Vcore_Handle Vcore_deiOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg)
{
    Int32               retVal = VPS_SOK;
    Vcore_DeiHandleObj *hObj = NULL;

    /* Only display and memory modes are supported */
    GT_assert(DeiCoreTrace,
        ((VCORE_OPMODE_DISPLAY == mode) || (VCORE_OPMODE_MEMORY == mode)));
    /* Check for maximum channel supported per handle */
    GT_assert(DeiCoreTrace, (numCh <= VCORE_DEI_MAX_CHANNELS));
    /* There should be atleast one channel */
    GT_assert(DeiCoreTrace, (0u != numCh));
    /* For display operation, only one channel should be allowed */
    GT_assert(DeiCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (VCORE_MAX_DISPLAY_CH < numCh))));

    /* Allocate a handle object */
    hObj = vcoreDeiAllocHandleObj(instId, mode);
    if (NULL != hObj)
    {
        /* Allocate channel objects */
        retVal = vcoreDeiAllocChObjs(hObj, numCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Channel allocation failed!!\n");
            /* Free-up handle object if channel allocation failed */
            vcoreDeiFreeHandleObj(hObj);
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
        GT_0trace(DeiCoreTrace, GT_ERR, "Handle allocation failed!!\n");
    }

    return ((Vcore_Handle) hObj);
}



/**
 *  Vcore_deiClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiClose(Vcore_Handle handle)
{
    Int32                   retVal = VPS_EFAIL;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Free channel and handle objects */
    retVal = vcoreDeiFreeChObjs(hObj, hObj->numCh);
    retVal |= vcoreDeiFreeHandleObj(hObj);
    if (VPS_SOK != retVal)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Free memory failed!!\n");
    }

    return (retVal);
}



/**
 *  Vcore_deiGetProperty
 *  \brief Gets the core properties of the DEI path core.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param property     [OUT] Pointer to which the DEI path core properties
 *                      to be copied.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property)
{
    Vcore_DeiInstObj       *instObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != property));
    instObj = ((Vcore_DeiHandleObj *) handle)->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    VpsUtils_memcpy(property, &VcoreDeiProperty, sizeof(Vcore_Property));
    if (TRUE == instObj->isHqDei)
    {
        property->name = VCORE_TYPE_DEI_HQ;
    }

    return (VPS_SOK);
}



/**
 *  Vcore_deiSetParams
 *  \brief Sets the DEI core parameters for a given channel.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the params should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param prms         [IN] Pointer to the params information. This should
 *                      point to a valid Vcore_DeiParams structure.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiSetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                const Void *prms)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt, startCh, numCh;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiInstObj       *instObj;
    Vcore_DeiHandleObj     *hObj;
    const Vcore_DeiParams  *corePrms;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != prms));
    hObj = (Vcore_DeiHandleObj *) handle;
    corePrms = (Vcore_DeiParams *) prms;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the parameters are valid */
        retVal = vcoreDeiCheckParams(instObj, corePrms);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Invalid Parameters!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE == hObj->perChCfg)
        {
            /* If configuration is per channel, then set the params only for
             * that channel */
            startCh = chNum;
            numCh = 1u;
        }
        else
        {
            /* If configuration is per handle, then set the params for all
             * the channels in the handle */
            startCh = 0u;
            numCh = hObj->numCh;
        }

        for (chCnt = startCh; chCnt < (startCh + numCh); chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            /* NULL pointer check */
            GT_assert(DeiCoreTrace, (NULL != chObj));

            /* Set the parameters for each channel */
            retVal = vcoreDeiSetChParams(instObj, chObj, corePrms);
            if (VPS_SOK != retVal)
            {
                GT_1trace(DeiCoreTrace, GT_ERR,
                    "Set parameter failed for channel %d!!\n", chCnt);
                break;
            }
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiGetParams
 *  \brief Gets the DEI core parameters for a given channel.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel from which the params should be read.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param prms         [OUT] Pointer to the copied params information. This
 *                      should point to a valid Vcore_DeiParams structure.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiGetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                Void *prms)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != prms));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        /* Copy the params */
        VpsUtils_memcpy(prms, &chObj->corePrms, sizeof(Vcore_DeiParams));
    }

    return (retVal);
}



/**
 *  Vcore_deiCreateLayout
 *  \brief Creates a mosaic layout for a given channel. This allocates memory
 *  for storing the layout information and if successful then assigns a valid
 *  layout ID to layoutId parameter. This should be used for all future
 *  reference to the created layout.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the config is be set.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param multiWinPrms [IN] Pointer to the mosaic config information.
 *                      This parameter should be non-NULL.
 *  \param layoutId     [OUT] Pointer to the layout ID where the core ID of
 *                      created layout will be populated on success. Else this
 *                      will be set to VCORE_INVALID_LAYOUT_ID.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiCreateLayout(Vcore_Handle handle,
                                   UInt32 chNum,
                                   const Vps_MultiWinParams *multiWinPrms,
                                   UInt32 *layoutId)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != multiWinPrms));
    GT_assert(DeiCoreTrace, (NULL != layoutId));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the requested mosaic config is valid */
        retVal = vcoreDeiCheckMosaicCfg(multiWinPrms);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Invalid Mosaic Configuration!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        /* Format should be set first as the frame dimension used in mosaic
         * layout is set using set format function */
        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before creating layouts!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Create the layout */
        retVal = vcoreDeiCreateLayout(
                     hObj->instObj,
                     chObj,
                     multiWinPrms,
                     layoutId);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Create layout failed!!\n");
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiDeleteLayout
 *  \brief Deletes the mosaic layout as identified by the layout ID.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the config is be set.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param layoutId     [IN] ID of the layout to be deleted.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiDeleteLayout(Vcore_Handle handle,
                                   UInt32 chNum,
                                   UInt32 layoutId)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        /* Delete the layout */
        retVal = vcoreDeiDeleteLayout(hObj->instObj, chObj, layoutId);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Delete layout failed!!\n");
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiGetDescInfo
 *  \brief Depending on the params set and the layout ID, returns the number of
 *  data descriptors and config overlay memory needed for the actual driver to
 *  allocate memory.
 *  Params should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the info is required.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param layoutId     [IN] Layout ID for which to return the information.
 *                      For non-mosaic configuration, this should be set to
 *                      VCORE_DEFAULT_LAYOUT_ID. For mosaic configuration, this
 *                      should be a valid layout ID as returned by create layout
 *                      function.
 *  \param descInfo     [OUT] Pointer to the number of data descriptor and
 *                      config overlay memory size information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiHandleObj     *hObj;
    Vcore_DeiMosaicInfo    *mInfo;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descInfo));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before getting the descriptor info!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (VCORE_DEFAULT_LAYOUT_ID == layoutId)
        {
            /* Copy descriptor information from non-mosaic info */
            VpsUtils_memcpy(
                (Ptr) descInfo,
                &chObj->descInfo,
                sizeof(Vcore_DescInfo));
        }
        else
        {
            /* Get the layout object for the requested layout */
            mInfo = vcoreDeiGetLayoutObj(chObj, layoutId);
            if (NULL == mInfo)
            {
                GT_0trace(DeiCoreTrace, GT_ERR, "Invalid layout ID!!\n");
                retVal = VPS_EINVALID_PARAMS;
            }
            else
            {
                /* Copy descriptor information from mosaic object */
                VpsUtils_memcpy(
                    (Ptr) descInfo,
                    &mInfo->descInfo,
                    sizeof(Vcore_DescInfo));
            }
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiGetDeiCtxInfo
 *  \brief Depending on the params set, returns the number of DEI context
 *  buffers needed for the actual driver to allocate memory.
 *  Params should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the info is required.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for M2M operation.
 *  \param ctxInfo      [OUT] Pointer to the context buffer information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiGetDeiCtxInfo(Vcore_Handle handle,
                                    UInt32 chNum,
                                    Vcore_DeiCtxInfo *ctxInfo)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != ctxInfo));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before getting the descriptor info!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Copy the context buffer information */
        VpsUtils_memcpy(
            (Ptr) ctxInfo,
            &chObj->ctxInfo,
            sizeof(Vcore_DeiCtxInfo));
    }

    return (retVal);
}



/**
 *  Vcore_deiSetFsEvent
 *  \brief Program the client's frame start.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
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
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiSetFsEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before setting the frame start event!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Set the frame start event */
        chObj->fsEvent = fsEvent;
        retVal = vcoreDeiSetFsEvent(hObj->instObj, chObj, descMem);
    }

    return (retVal);
}



/**
 *  Vcore_deiControl
 *  \brief To handle control Command.
 *
 *  \param handle       DEI Path Core handle.
 *  \param cmd          Control Command
 *  \param args         Command Specific Argument
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiControl(Vcore_Handle handle, UInt32 cmd, Ptr args)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    hObj = (Vcore_DeiHandleObj *) handle;

    switch (cmd)
    {
        case VCORE_IOCTL_SET_DEIHQ_CFG:
            GT_assert(DeiCoreTrace, (NULL != args));
            if (TRUE == hObj->instObj->isHqDei)
            {
                retVal = vcoreDeiHqSetAdvCfgIoctl(
                            hObj,
                            (Vps_DeiHqRdWrAdvCfg *) args);
            }
            else
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "IOCTL not supported for this instance\n");
                retVal = VPS_EUNSUPPORTED_CMD;
            }
            break;

        case VCORE_IOCTL_GET_DEIHQ_CFG:
            GT_assert(DeiCoreTrace, (NULL != args));
            if (TRUE == hObj->instObj->isHqDei)
            {
                retVal = vcoreDeiHqGetAdvCfgIoctl(
                            hObj,
                            (Vps_DeiHqRdWrAdvCfg *) args);
            }
            else
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "IOCTL not supported for this instance\n");
                retVal = VPS_EUNSUPPORTED_CMD;
            }
            break;

        case VCORE_IOCTL_SET_DEI_CFG:
            GT_assert(DeiCoreTrace, (NULL != args));
            if (FALSE == hObj->instObj->isHqDei)
            {
                retVal = vcoreDeiSetAdvCfgIoctl(
                            hObj,
                            (Vps_DeiRdWrAdvCfg *) args);
            }
            else
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "IOCTL not supported for this instance\n");
                retVal = VPS_EUNSUPPORTED_CMD;
            }
            break;

        case VCORE_IOCTL_GET_DEI_CFG:
            GT_assert(DeiCoreTrace, (NULL != args));
            if (FALSE == hObj->instObj->isHqDei)
            {
                retVal = vcoreDeiGetAdvCfgIoctl(
                            hObj,
                            (Vps_DeiRdWrAdvCfg *) args);
            }
            else
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "IOCTL not supported for this instance\n");
                retVal = VPS_EUNSUPPORTED_CMD;
            }
            break;

        case VCORE_IOCTL_SET_SC_CFG:
            GT_assert(DeiCoreTrace, (NULL != args));
            retVal = vcoreDeiScSetAdvCfgIoctl(hObj, (Vcore_ScCfgParams *) args);
            break;

        case VCORE_IOCTL_GET_SC_CFG:
            GT_assert(DeiCoreTrace, (NULL != args));
            retVal = vcoreDeiScGetAdvCfgIoctl(hObj, (Vcore_ScCfgParams *) args);
            break;

        default:
            GT_0trace(DeiCoreTrace, GT_ERR, "Unsupported IOCTL\n");
            retVal = VPS_EUNSUPPORTED_CMD;
            break;
    }

    return (retVal);
}



/**
 *  Vcore_deiProgramDesc
 *  \brief Programs the data descriptor for a given channel depending on the
 *  layout ID passed. This will not program the internal context descriptors.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for M2M operation.
 *  \param layoutId     [IN] Layout ID for which to program the descriptor.
 *                      For non-mosaic configuration, this should be set to
 *                      VCORE_DEFAULT_LAYOUT_ID. For mosaic configuration, this
 *                      should be a valid layout ID as returned by create layout
 *                      function.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiInstObj       *instObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before programming the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Program descriptors */
        if (VCORE_DEFAULT_LAYOUT_ID == layoutId)
        {
            retVal = vcoreDeiProgramDesc(instObj, chObj, descMem);
        }
        else
        {
            retVal =
                vcoreDeiProgramMosaicDesc(instObj, chObj, layoutId, descMem);
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Configure previous field descriptors */
        retVal = vcoreDeiProgramPrevFldDesc(instObj, chObj, descMem);
        retVal |= vcoreDeiProgramCurOutDesc(instObj, chObj, descMem);
        retVal |= vcoreDeiProgramMvDesc(instObj, chObj, descMem);
        retVal |= vcoreDeiProgramMvstmDesc(instObj, chObj, descMem);
    }

    return (retVal);
}



/**
 *  Vcore_deiProgramReg
 *  \brief Programs the registers for each of the HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
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
static Int32 Vcore_deiProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  halCnt;
    Void                   *cfgOvlyPtr = NULL;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiInstObj       *instObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before programming the overlay memory!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != descMem)
        {
            /* Check if overlay pointers are non-NULL for required overlay */
            GT_assert(DeiCoreTrace,
                !((0u != chObj->descInfo.shadowOvlySize)
                && (NULL == descMem->shadowOvlyMem)));

            /* Create overlay memory for each of the HAL */
            for (halCnt = 0u; halCnt < VCORE_DEI_MAX_HAL; halCnt++)
            {
                cfgOvlyPtr = (Void *)
                    ((UInt32) descMem->shadowOvlyMem +
                        instObj->ovlyOffset[halCnt]);
                if (NULL != instObj->halHandle[halCnt])
                {
                    /* NULL pointer check */
                    GT_assert(DeiCoreTrace,
                        (NULL != instObj->createCfgOvly[halCnt]));
                    retVal = instObj->createCfgOvly[halCnt](
                                 instObj->halHandle[halCnt],
                                 cfgOvlyPtr);
                    if (VPS_SOK != retVal)
                    {
                        GT_1trace(DeiCoreTrace, GT_ERR,
                            "HAL Create Overlay Failed for HAL Id: %d!!\n",
                            halCnt);
                        break;
                    }
                }
            }
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Program the registers */
        retVal = vcoreDeiProgramReg(instObj, chObj, descMem);
    }

    return (retVal);
}



/**
 *  Vcore_deiUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frame        [IN] Pointer to the FVID frame containing the buffer
 *                      address. This parameter should be non-NULL.
 *  \param fid          [IN] FID to be programmed in the descriptor.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chNum;
    Void                   *descPtr;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiInstObj       *instObj;
    Vcore_DeiHandleObj     *hObj;
    UInt32                  descOffset;
    UInt32                  cnt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    GT_assert(DeiCoreTrace, (NULL != frame));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

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
            GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
        {
            /*
             *  Caution:
             *  Next statement relies on the fact that VCORE_DEI_FLD0LUMA_IDX
             *  and VCORE_DEI_FLD0CHROMA_IDX are next to each other.
             */
            /* Update Y and C data descriptor */
            descOffset = chObj->descOffset[VCORE_DEI_FLD0LUMA_IDX + cnt];
            descPtr = descMem->inDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            if (TRUE == chObj->ctxInfo.isDeinterlacing)
            {
                /* Use the field mode address index when deinterlacing */
                VpsHal_vpdmaSetAddress(
                    descPtr,
                    fid,
                    frame->addr[FVID2_FIELD_MODE_ADDR_IDX]
                               [chObj->bufIndex[cnt]]);
            }
            else
            {
                /* Use appropriate frame/field address index when in normal
                 * mode */
                VpsHal_vpdmaSetAddress(
                    descPtr,
                    fid,
                    frame->addr[fid][chObj->bufIndex[cnt]]);
            }
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiUpdateMultiDesc
 *  \brief Updates the multiple window buffer addresses provided in the frame
 *  list structure to the descriptor memories.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param layoutId     [IN] Layout ID for which to update the descriptor.
 *                      For non-mosaic configuration, this should be set to
 *                      VCORE_DEFAULT_LAYOUT_ID. For mosaic configuration, this
 *                      should be a valid layout ID as returned by create layout
 *                      function.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frameList    [IN] Pointer to the FVID frame containing the buffer
 *                      address. This parameter should be non-NULL.
 *  \param fid          [IN] FID to be programmed in the descriptor.
 *  \param startIdx     [IN] Index to the frame list from where the core will
 *                      start using the frames for updating multi window
 *                      descriptors.
 *  \param frmConsumed  [OUT] Number of frames consumed from frame list.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiUpdateMultiDesc(Vcore_Handle handle,
                                      UInt32 layoutId,
                                      const Vcore_DescMem *descMem,
                                      const FVID2_FrameList *frameList,
                                      UInt32 fid,
                                      UInt32 startIdx,
                                      UInt32 *frmConsumed)
{
    Int32                       retVal = VPS_SOK;
    Void                       *descPtr, *bufPtr;
    Bool                        swapLumaChroma = FALSE;
    UInt32                      cpuRev, platformId;
    UInt32                      descOffset, bufOffset, tempOffset;
    UInt32                      cnt, winCnt;
    UInt32                      chNum;
    FVID2_Frame                *frame;
    Vcore_DeiChObj             *chObj;
    Vcore_DeiInstObj           *instObj;
    Vcore_DeiHandleObj         *hObj;
    Vcore_DeiMosaicInfo        *mInfo;
    const Vps_WinFormat        *winFmt;
    VpsHal_VpdmaMosaicWinFmt   *mulWinFmt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    GT_assert(DeiCoreTrace, (NULL != frameList));
    GT_assert(DeiCoreTrace, (NULL != frmConsumed));

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

    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    if (NULL != frameList->frames[startIdx])
    {
        /* For display mode, channel number is always zero. So ignore it. */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            chNum = 0u;
        }
        else
        {
            chNum = frameList->frames[startIdx]->channelNum;
            /* Check if the channel is within the range allocated during
             * open */
            if (chNum >= hObj->numCh)
            {
                GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
                retVal = VPS_EOUT_OF_RANGE;
            }
        }
    }
    else
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Null Pointer!!\n");
        retVal = VPS_EBADARGS;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Check if layout ID is valid */
        if (VCORE_DEFAULT_LAYOUT_ID == layoutId)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Update multiwin desc fn called in non-multi window mode!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Get the layout object for the requested layout */
        mInfo = vcoreDeiGetLayoutObj(chObj, layoutId);
        if (NULL == mInfo)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Invalid layout ID!!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
        else
        {
            /* Check if numFrames is greater than required number of windows */
            if (frameList->numFrames <
                (startIdx + mInfo->appMulWinPrms.numWindows))
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "Num frames not sufficient for required mosaic mode!!\n");
                retVal = VPS_EOUT_OF_RANGE;
            }
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Update the first row descriptor to use In data memory */
        descOffset = 0u;
        for (winCnt = 0u; winCnt < mInfo->mulWinPrms.numSplitWindows; winCnt++)
        {
            mulWinFmt = &mInfo->mulWinFmt[winCnt];
            winFmt = mulWinFmt->ipWindowFmt;
            GT_assert(DeiCoreTrace, (NULL != winFmt));

            /* Get the frame index for this descriptor */
            GT_assert(DeiCoreTrace,
                (mulWinFmt->frameIndex < mInfo->appMulWinPrms.numWindows));
            frame = frameList->frames[startIdx + mulWinFmt->frameIndex];
            if (NULL == frame)
            {
                GT_0trace(DeiCoreTrace, GT_ERR, "Null Pointer!!\n");
                retVal = VPS_EBADARGS;
                break;
            }

            for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
            {
                /*
                 *  Caution:
                 *  Next statement relies on the fact that
                 *  VCORE_DEI_FLD0LUMA_IDX and VCORE_DEI_FLD0CHROMA_IDX are
                 *  next to each other.
                 */
                if (winCnt < mInfo->mulWinPrms.numWindowsFirstRow)
                {
                    /* Program luma first and then chroma for first row */
                    tempOffset = descOffset + cnt;
                    /* Get descriptor pointer for the first row */
                    descPtr = descMem->inDataDesc[tempOffset];
                }
                else
                {
                    if ((winCnt == mInfo->mulWinPrms.numWindowsFirstRow) &&
                        (0u == cnt))
                    {
                        /* Reset the count when moving to 2nd row luma */
                        descOffset = 0u;
                    }

                    tempOffset = descOffset + cnt;
                    if (TRUE == swapLumaChroma)
                    {
                        /* Program chroma first and then luma for non-first
                         * row */
                        tempOffset =
                            descOffset + (instObj->numDescPerWindow - 1u) - cnt;
                    }

                    /* Get descriptor pointer for the non-first row */
                    descPtr = descMem->multiWinDataDesc[tempOffset];
                }
                GT_assert(DeiCoreTrace, (NULL != descPtr));

                /* Calculate buffer address */
                bufPtr = frame->addr[fid][chObj->bufIndex[cnt]];
                if (NULL == bufPtr)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR, "Null Pointer!!\n");
                    retVal = VPS_EBADARGS;
                    break;
                }

                /* Calculate buffer offset based on pitch and format */
                if (FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat)
                {
                    /* YUYV422 interleaved format */
                    bufOffset = mulWinFmt->bufferOffsetX * 2u;
                }
                else
                {
                    /* YUV422/YUV420 semi planar format */
                    bufOffset = mulWinFmt->bufferOffsetX;
                }
                bufOffset += (mulWinFmt->bufferOffsetY *
                                winFmt->pitch[chObj->bufIndex[cnt]]);

                /* Update Y and C data descriptor */
                bufPtr = (Void *) ((UInt32) bufPtr + bufOffset);
                VpsHal_vpdmaSetAddress(descPtr, fid, bufPtr);
            }
            descOffset += instObj->numDescPerWindow;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Update number of frames consumed */
        *frmConsumed += mInfo->appMulWinPrms.numWindows;
    }

    return (retVal);
}



/**
 *  Vcore_deiUpdateContext
 *  \brief Updates the DEI context buffer addresses provided in the
 *  update context structure to the descriptor memories.
 *
 *  \param handle       [IN] DEI Path Core handle.
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
 *  \param context      [IN] Pointer to the Vcore_DeiUpdateContext structure
 *                      containing the context buffer address and other
 *                      information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiUpdateContext(Vcore_Handle handle,
                                    UInt32 chNum,
                                    const Vcore_DescMem *descMem,
                                    const Void *ctx)
{
    Int32                           retVal = VPS_SOK;
    UInt32                          cnt, mvCnt, fldCnt;
    UInt32                          tempIdx, mvstmIdx;
    Void                           *descPtr;
    UInt32                          descOffset;
    UInt32                          prevFldDataFmt;
    UInt16                          frameWidth, frameHeight;
    UInt16                          transWidth, transHeight;
    Vcore_DeiChObj                 *chObj;
    Vcore_DeiInstObj               *instObj;
    Vcore_DeiHandleObj             *hObj;
    const Vcore_DeiUpdateContext   *context;
#ifdef VPS_HAL_INCLUDE_DCOMPR
    UInt32                          decompIdx;
    Void                           *cfgOvlyPtr;
#endif

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    GT_assert(DeiCoreTrace, (NULL != ctx));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));
    context = (Vcore_DeiUpdateContext *) ctx;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
                instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
            (VPSHAL_VPDMA_CHANNEL_INVALID ==
                instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
        {
            /* No write channel available, use the data format of input */
            prevFldDataFmt = chObj->corePrms.fmt.dataFormat;
        }
        else
        {
            /* Use YUV422SP if write descriptor is present */
            prevFldDataFmt = FVID2_DF_YUV422SP_UV;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Update the previous field descriptors */
        for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_PREV_FLD; fldCnt++)
        {
            /*
             *  Caution:
             *  This relies on the fact that the previous fields are in order
             *  starting from VCORE_DEI_FLD1LUMA_IDX.
             */
            tempIdx = VCORE_DEI_FLD1LUMA_IDX +
                (fldCnt * VCORE_DEI_NUM_DESC_PER_FIELD);
            for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
            {
                if (TRUE == chObj->isDescReq[tempIdx + cnt])
                {
                    /* Update data descriptor */
                    descOffset = chObj->descOffset[tempIdx + cnt];
                    descPtr = descMem->inDataDesc[descOffset];
                    GT_assert(DeiCoreTrace, (NULL != descPtr));
                    if (((VPS_DEIHQ_CTXMODE_APP_N_1
                            == chObj->corePrms.deiHqCtxMode)
                        && (0u == fldCnt)
                        && (FVID2_DF_YUV422I_YUYV
                            == chObj->corePrms.inFmtFldN_1.dataFormat)) ||
                            (FVID2_DF_YUV422I_YUYV == prevFldDataFmt))
                    {
                        GT_assert(DeiCoreTrace,
                            (NULL != context->prevInBuf
                                        [fldCnt][FVID2_YUV_INT_ADDR_IDX]));
                        /* For YUV422 N-1 input, same address should be
                         * programmed for chroma descriptor */
                        VpsHal_vpdmaSetAddress(
                            descPtr,
                            context->prevInFid[fldCnt],
                            context->prevInBuf[fldCnt][FVID2_YUV_INT_ADDR_IDX]);
                    }
                    else
                    {
                        GT_assert(DeiCoreTrace,
                            (NULL != context->prevInBuf[fldCnt][cnt]));

                        VpsHal_vpdmaSetAddress(
                            descPtr,
                            context->prevInFid[fldCnt],
                            context->prevInBuf[fldCnt][cnt]);
                    }

                    /* Enable/disable 1D if decompression is enabled/disabled */
                    VpsHal_vpdmaSet1DMem(
                        descPtr,
                        context->dcomprEnable[fldCnt]);

                    if (TRUE == context->dcomprEnable[fldCnt])
                    {
                        /* Update frame size if decompressor is enabled */
                        VpsHal_vpdmaSetDecomprFrameSize(
                            descPtr,
                            context->frameSize[fldCnt][cnt],
                            context->transferSize[fldCnt][cnt]);
                    }
                    else
                    {
                        /* Restore the values if disabled */
                        transWidth = chObj->corePrms.fmt.width;
                        frameWidth = chObj->corePrms.fmt.width;

                        /* In the Progressive mode, since entire frame is
                         * queued, size of field is half of frame size */
                        if (FVID2_SF_PROGRESSIVE ==
                            chObj->corePrms.fmt.scanFormat)
                        {
                            transHeight = chObj->corePrms.fmt.height / 2u;
                            frameHeight = chObj->corePrms.fmt.height / 2u;
                        }
                        else
                        {
                            transHeight = chObj->corePrms.fmt.height;
                            frameHeight = chObj->corePrms.fmt.height;
                        }

                        /* Chroma height is half of Luma for YUV420 */
                        if ((VCORE_DEI_CBCR_IDX == cnt) &&
                            (FVID2_DF_YUV420SP_UV == prevFldDataFmt))
                        {
                            transHeight /= 2u;
                            frameHeight /= 2u;
                        }

                        VpsHal_vpdmaSetFrameDim(
                            descPtr,
                            frameWidth,
                            frameHeight);
                        VpsHal_vpdmaSetTransferDim(
                            descPtr,
                            transWidth,
                            transHeight);
                    }
                }
            }

#ifdef VPS_HAL_INCLUDE_DCOMPR
            /* Enable/disable decompression */
            GT_assert(DeiCoreTrace,
                (VCORE_DEI_MAX_DCOMPR <= VCORE_DEI_MAX_PREV_FLD));
            chObj->dcomprHalCfg[fldCnt].enable = context->dcomprEnable[fldCnt];
            decompIdx = fldCnt + VCORE_DEI_DCOMPR0_IDX;
            if (NULL != instObj->halHandle[decompIdx])
            {
                GT_assert(DeiCoreTrace,
                    (NULL != descMem->shadowOvlyMem));
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[decompIdx]);
                retVal = VpsHal_dcomprSetConfig(
                             instObj->halHandle[decompIdx],
                             &chObj->dcomprHalCfg[fldCnt],
                             cfgOvlyPtr);
                GT_assert(DeiCoreTrace, (VPS_SOK == retVal));
            }
#endif
        }

        /* Update the current field/frame out descriptors */
        for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_OUT_FLD; fldCnt++)
        {
            /*
             *  Caution:
             *  This relies on the fact that the previous fields are in order
             *  starting from VCORE_DEI_FLD1LUMA_IDX.
             */
            tempIdx = VCORE_DEI_WRLUMA_IDX +
                (fldCnt * VCORE_DEI_NUM_DESC_PER_FIELD);
            for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
            {
                if (TRUE == chObj->isDescReq[tempIdx + cnt])
                {
                    /* Update current field out data descriptor */
                    descOffset = chObj->descOffset[tempIdx + cnt];
                    descPtr = descMem->outDataDesc[descOffset];
                    GT_assert(DeiCoreTrace, (NULL != descPtr));
                    GT_assert(DeiCoreTrace,
                        (NULL != context->curOutBuf[fldCnt][cnt]));
                    VpsHal_vpdmaSetAddress(
                        descPtr,
                        0u,     /* FID is reserved for outbound descriptors */
                        context->curOutBuf[fldCnt][cnt]);

#ifdef VPS_HAL_INCLUDE_COMPR
                    /* Update write descriptor if compressor is enabled */
                    if (TRUE == chObj->comprHalCfg[fldCnt].enable)
                    {
                        GT_assert(DeiCoreTrace,
                            (NULL != context->curOutWriteDesc[fldCnt][cnt]));
                        VpsHal_vpdmaSetDescWriteAddr(
                            descPtr,
                            context->curOutWriteDesc[fldCnt][cnt]);
                    }
                    /* Set enable write descriptor depending on compressor
                     * enable/disable */
                    VpsHal_vpdmaSetWriteDesc(
                        descPtr,
                        chObj->comprHalCfg[fldCnt].enable);
#endif
                }
            }
        }

        /* Update the MV In descriptors */
        for (mvCnt = 0u; mvCnt < VCORE_DEI_MAX_MV_IN; mvCnt++)
        {
            /*
             *  Caution:
             *  This relies on the fact that the MVs are in order
             *  starting from VCORE_DEI_MV1_IDX.
             */
            tempIdx = VCORE_DEI_MV1_IDX + mvCnt;
            if (TRUE == chObj->isDescReq[tempIdx])
            {
                /* Update MV In data descriptor */
                descOffset = chObj->descOffset[tempIdx];
                descPtr = descMem->inDataDesc[descOffset];
                GT_assert(DeiCoreTrace, (NULL != descPtr));
                GT_assert(DeiCoreTrace, (NULL != context->mvInBuf[mvCnt]));
                VpsHal_vpdmaSetAddress(
                    descPtr,
                    context->mvInFid[mvCnt],
                    context->mvInBuf[mvCnt]);
            }
        }

        /* Update the MV Out descriptor */
        if (TRUE == chObj->isDescReq[VCORE_DEI_MVOUT_IDX])
        {
            /* Update MV Out data descriptor */
            descOffset = chObj->descOffset[VCORE_DEI_MVOUT_IDX];
            descPtr = descMem->outDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            GT_assert(DeiCoreTrace, (NULL != context->mvOutBuf));
            VpsHal_vpdmaSetAddress(
                descPtr,
                0u,             /* FID is reserved for outbound descriptors */
                context->mvOutBuf);
        }

        /* Update the MVSTM In descriptors */
        if (VPS_DEIHQ_CTXMODE_APP_N_1 == chObj->corePrms.deiHqCtxMode)
        {
            mvstmIdx = 0u;
        }
        else
        {
            mvstmIdx = 1u;
        }
        for (mvCnt = 0u; mvCnt < VCORE_DEI_MAX_MVSTM_IN; mvCnt++)
        {
            /*
             *  Caution:
             *  This relies on the fact that the MVSTMs are in order
             *  starting from VCORE_DEI_MVSTM_IDX.
             */
            tempIdx = VCORE_DEI_MVSTM_IDX + mvCnt;
            if (TRUE == chObj->isDescReq[tempIdx])
            {
                /* Update MVSTM In data descriptor */
                descOffset = chObj->descOffset[tempIdx];
                descPtr = descMem->inDataDesc[descOffset];
                GT_assert(DeiCoreTrace, (NULL != descPtr));
                GT_assert(DeiCoreTrace, context->mvstmInBuf[mvCnt + mvstmIdx]);

                /* Buffer at the index 0 is for storing mvstm of previous
                   field. Since MVSTM needs two field delayed data, it should
                   take buffer from index 1. */
                VpsHal_vpdmaSetAddress(
                    descPtr,
                    context->mvstmInFid[mvCnt + mvstmIdx],
                    context->mvstmInBuf[mvCnt + mvstmIdx]);
            }
        }

        /* Update the MVSTM Out descriptor */
        if (TRUE == chObj->isDescReq[VCORE_DEI_MVSTMOUT_IDX])
        {
            /* Update MV Out data descriptor */
            descOffset = chObj->descOffset[VCORE_DEI_MVSTMOUT_IDX];
            descPtr = descMem->outDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            GT_assert(DeiCoreTrace, (NULL != context->mvstmOutBuf));
            VpsHal_vpdmaSetAddress(
                descPtr,
                0u,             /* FID is reserved for outbound descriptors */
                context->mvstmOutBuf);
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiUpdateRtMem
 *  \brief Updates the DEI runtime parameters in the descriptor as well as
 *  in register overlay memories.
 *
 *  \param handle       [IN] DEI Path Core handle.
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
 *  \param rtPrms       [IN] Pointer to the Vcore_DeiRtParams structure
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
static Int32 Vcore_deiUpdateRtMem(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  UInt32 chNum,
                                  const Void *rtPrms,
                                  UInt32 frmIdx)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      chCnt, startCh, numCh;
    Vcore_DeiChObj             *chObj;
    Vcore_DeiInstObj           *instObj;
    Vcore_DeiHandleObj         *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
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
            GT_assert(DeiCoreTrace, (NULL != chObj));

            if (TRUE != chObj->state.isParamsSet)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "Set the parameter before updating the descriptors!!\n");
                retVal = VPS_EFAIL;
                break;
            }

            if (NULL != rtPrms)
            {
                retVal = vcoreDeiUpdateRtPrms(instObj, chObj, rtPrms);
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "Update RT params failed for channel %d!!\n", chCnt);
                    break;
                }
            }

            retVal = vcoreDeiUpdateRtDescMem(instObj, chObj, descMem);
            if (VPS_SOK != retVal)
            {
                GT_1trace(DeiCoreTrace, GT_ERR,
                    "Update RT memory failed for channel %d!!\n", chCnt);
                break;
            }
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiProgramScCoeff
 *  \brief Programs the coefficient overlay memory with the scalar coefficients
 *  according to the parameter passed. The upper driver has to submit the
 *  programmed overlay to the VPDMA to do the actual coefficient programming.
 *
 *  \param handle       [IN] DEI Path Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in M2M mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for M2M operation.
 *  \param coeffPrms    [IN] Pointer to the coeff information.
 *                      This parameter should be non-NULL. The pointers to the
 *                      coeff overlays are returned in this structure.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiProgramScCoeff(Vcore_Handle handle,
                                     UInt32 chNum,
                                     Vcore_ScCoeffParams *coeffPrms)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiInstObj       *instObj;
    Vcore_DeiHandleObj     *hObj;
    VpsHal_ScCoeffConfig    coeffCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != coeffPrms));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before programming the overlay memory!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_SC_IDX])
        {
            if (NULL != coeffPrms->userCoeffPtr)
            {
                retVal = VpsHal_scCreateUserCoeffOvly(
                             instObj->halHandle[VCORE_DEI_SC_IDX],
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
                         instObj->halHandle[VCORE_DEI_SC_IDX],
                         &coeffCfg,
                         &(coeffPrms->horzCoeffMemPtr),
                         &(coeffPrms->vertCoeffMemPtr),
                         &(coeffPrms->vertBilinearCoeffMemPtr));
            }
        }
        else
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Scalar is not present to be programmed!!\n");
            retVal = VPS_EFAIL;
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiUpdateMode
 *  \brief Updates DEI Mode as per the number of Frames.
 *
 *  \param handle       [IN] DEI Path Core handle.
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
 *  \param prevFldState [IN] Represents the previous field state - used in
 *                      DEI reset sequence to build up the DEI context
 *                      and state.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_deiUpdateMode(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem,
                                 UInt32 prevFldState)
{
    Int32                   retVal = VPS_SOK;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiInstObj       *instObj;
    Vcore_DeiHandleObj     *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    hObj = (Vcore_DeiHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (TRUE == instObj->isHqDei)
        {
            retVal = vcoreDeiHqUpdateMode(
                         instObj,
                         chObj,
                         descMem,
                         prevFldState);
        }
        else
        {
            retVal = vcoreDeiUpdateMode(
                         instObj,
                         chObj,
                         descMem,
                         prevFldState);
        }
    }

    return (retVal);
}



/**
 *  Vcore_deiGetScFactorConfig
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
static Int32 Vcore_deiGetScFactorConfig(Vcore_Handle handle,
                                        UInt32 chNum,
                                        VpsHal_ScFactorConfig *scFactorConfig,
                                        const Vcore_DescMem *descMem)
{
    Int32 retVal = VPS_SOK;
    Void *cfgOvlyPtr = NULL;
    Vcore_DeiHandleObj *hObj;
    Vcore_DeiChObj *chObj;
    VpsHal_ScConfig *scHalCfg;

    /* Check for errors */
    GT_assert(DeiCoreTrace, (NULL != handle));
    GT_assert(DeiCoreTrace, (NULL != scFactorConfig));
    GT_assert(SwpCoreTrace, (NULL != descMem));
    GT_assert(SwpCoreTrace, (NULL != descMem->shadowOvlyMem));

    hObj = (Vcore_DeiHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    else
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(DeiCoreTrace, (NULL != chObj));

        if (NULL != hObj->instObj->halHandle[VCORE_DEI_SC_IDX])
        {
            /* Call the HAL function to get the scaling factor config. */
            scHalCfg = &chObj->scHalCfg;
            cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                + hObj->instObj->ovlyOffset[VCORE_DEI_SC_IDX]);
            retVal = VpsHal_scGetScFactorConfig(
                        hObj->instObj->halHandle[VCORE_DEI_SC_IDX],
                        scHalCfg,
                        scFactorConfig,
                        cfgOvlyPtr);
        }
        else
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
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
 *  vcoreDeiUpdateRtPrms
 *  \brief Update runtime parameters into channel instance.
 */
static Int32 vcoreDeiUpdateRtPrms(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj,
                                  const Vcore_DeiRtParams *rtPrms)
{
    UInt32                      retVal;
    UInt32                      cnt;
    VpsHal_ScConfig            *scHalCfg;
    Vcore_DeiParams             corePrms;
    const Vcore_DeiRtParams    *deiRtPrms;
    Vps_FrameParams            *inFrmPrms;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != rtPrms));

    scHalCfg = &chObj->scHalCfg;
    deiRtPrms = (const Vcore_DeiRtParams *) rtPrms;

    /* Copy the existing params and change only the required ones */
    VpsUtils_memcpy(&corePrms, &chObj->corePrms, sizeof(Vcore_DeiParams));

    /* Allow input resolution change only if compressor is disabled */
    inFrmPrms = NULL;
    if (NULL != deiRtPrms->inFrmPrms)
    {
        if (FALSE == corePrms.comprEnable[0u])
        {
            inFrmPrms = deiRtPrms->inFrmPrms;
        }
        else
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Ignoring IN Frame RT param since compressor is enabled\n");
        }
    }

    /* Copy the in frame params to core params */
    if (NULL != inFrmPrms)
    {
        corePrms.fmt.width = inFrmPrms->width;
        corePrms.fmt.height = inFrmPrms->height;
        /* For M2M mode, when dimension changes, change the frame dimensions as
         * well. This should not be done for display mode as frame width and
         * frame height represents display resolution and they never change. */
        if (VCORE_OPMODE_MEMORY == instObj->curMode)
        {
            corePrms.frameWidth = inFrmPrms->width;
            corePrms.frameHeight = inFrmPrms->height;
        }
        else if (0 == rtPrms->isMosaicMode)
        {
            /* For the non-mosaic display mode, frame size is same input size */
            corePrms.frameWidth = inFrmPrms->width;
            corePrms.frameHeight = inFrmPrms->height;
        }

        for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
        {
            corePrms.fmt.pitch[cnt] = inFrmPrms->pitch[cnt];
        }
        corePrms.memType = inFrmPrms->memType;
        corePrms.fmt.dataFormat = inFrmPrms->dataFormat;
    }

    /* Copy the position params to core params */
    if (NULL != rtPrms->posCfg)
    {
        corePrms.startX = rtPrms->posCfg->startX;
        corePrms.startY = rtPrms->posCfg->startY;
    }

    /* Copy the out frame params to core params */
    if (NULL != deiRtPrms->outFrmPrms)
    {
        corePrms.tarWidth = deiRtPrms->outFrmPrms->width;
        corePrms.tarHeight = deiRtPrms->outFrmPrms->height;
    }

    /* Copy the crop config to core params */
    if (NULL != deiRtPrms->scCropCfg)
    {
        corePrms.cropCfg.cropStartX = deiRtPrms->scCropCfg->cropStartX;
        corePrms.cropCfg.cropStartY = deiRtPrms->scCropCfg->cropStartY;
        corePrms.cropCfg.cropWidth = deiRtPrms->scCropCfg->cropWidth;
        corePrms.cropCfg.cropHeight = deiRtPrms->scCropCfg->cropHeight;
    }
    else if (NULL != inFrmPrms)
    {
        /* When input changes and if user did not modify the scalar crop
         * configuration, use the modified input params. */
        corePrms.cropCfg.cropStartX = 0u;
        corePrms.cropCfg.cropStartY = 0u;
        corePrms.cropCfg.cropWidth = inFrmPrms->width;
        if (TRUE == chObj->ctxInfo.isDeinterlacing)
        {
            corePrms.cropCfg.cropHeight = inFrmPrms->height * 2u;
        }
        else
        {
            corePrms.cropCfg.cropHeight = inFrmPrms->height;
        }
    }

    /* Copy the scalar runtime configuraton to core params */
    if (NULL != deiRtPrms->scRtCfg)
    {
        corePrms.scCfg.bypass = deiRtPrms->scRtCfg->scBypass;
    }

    /* Dummy data descriptor is not needed if frame size and buffer
     * bottom-right coordinates match */
    if (((corePrms.startX + corePrms.fmt.width) < corePrms.frameWidth) ||
        ((corePrms.startY + corePrms.fmt.height) < corePrms.frameHeight))
    {
        chObj->isDummyNeeded = TRUE;
    }
    else
    {
        chObj->isDummyNeeded = FALSE;
    }

    /* Check whether the updated RT params are valid */
    retVal = vcoreDeiCheckParams(instObj, &corePrms);
    if (VPS_SOK != retVal)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid runtime parameters!!\n");
    }
    else
    {
        /* Update the new configuration */
        retVal = vcoreDeiSetChParams(instObj, chObj, &corePrms);
        GT_assert(DeiCoreTrace, (VPS_SOK == retVal));

        /* Update scalar phase info from application provided data.
         * Caution: This should be done after calling vcoreDeiSetChParams()
         * as this resets the phInfoMode to default mode. */
        if ((NULL != deiRtPrms->scRtCfg) &&
            (TRUE == deiRtPrms->scRtCfg->scSetPhInfo))
        {
            scHalCfg->phInfoMode = VPS_SC_SET_PHASE_INFO_FROM_APP;
            scHalCfg->rowAccInc = deiRtPrms->scRtCfg->rowAccInc;
            scHalCfg->rowAccOffset = deiRtPrms->scRtCfg->rowAccOffset;
            scHalCfg->rowAccOffsetB = deiRtPrms->scRtCfg->rowAccOffsetB;
            scHalCfg->ravScFactor = deiRtPrms->scRtCfg->ravScFactor;
            scHalCfg->ravRowAccInit = deiRtPrms->scRtCfg->ravRowAccInit;
            scHalCfg->ravRowAccInitB = deiRtPrms->scRtCfg->ravRowAccInitB;
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiUpdateRtDescMem
 *  \brief Program vpdma inbound data descriptor based on channel instance
 *  parameters.
 */
static Int32 vcoreDeiUpdateRtDescMem(const Vcore_DeiInstObj *instObj,
                                     Vcore_DeiChObj *chObj,
                                     const Vcore_DescMem *descMem)
{
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    /* Re-program the registers */
    if (VCORE_OPMODE_MEMORY == instObj->curMode)
    {
        /* Check if overlay pointers are non-NULL for required overlay */
        GT_assert(DeiCoreTrace,
            !((0u != chObj->descInfo.shadowOvlySize)
            && (NULL == descMem->shadowOvlyMem)));

        retVal = vcoreDeiProgramReg(instObj, chObj, descMem);
    }
    else /* Display Mode: Direct MMR Write */
    {
        /* Display supports only change of size at run time,
           so there is no need to change all registers */
        retVal = vcoreDeiProgramFrmSizeReg(instObj, chObj, NULL);
    }

    if (VPS_SOK == retVal)
    {
        /* Re-program the descriptors */
        retVal = vcoreDeiProgramDesc(instObj, chObj, descMem);

        /* Re-Program Previous Descriptors and other context descriptors only
           if DEI is deinterlacing */
        if (TRUE == chObj->ctxInfo.isDeinterlacing)
        {
            /* Re-program context descriptors as well since we allow input
             * resolution change even if DEI is enabled. */
            retVal |= vcoreDeiProgramPrevFldDesc(instObj, chObj, descMem);
            retVal |= vcoreDeiProgramCurOutDesc(instObj, chObj, descMem);
            retVal |= vcoreDeiProgramMvDesc(instObj, chObj, descMem);
            retVal |= vcoreDeiProgramMvstmDesc(instObj, chObj, descMem);
        }
    }

    /* Update the frame start event only for M2M mode as we support runtime
     * change of input format through overlay memory.
     * This should not be done for display mode as this register is updated
     * directly using MMR access and could currupt the current frame because
     * this register is non-shadowed!! */
    if ((VPS_SOK == retVal) && (VCORE_OPMODE_MEMORY == instObj->curMode))
    {
        GT_assert(DeiCoreTrace, (NULL != descMem->nonShadowOvlyMem));
        retVal = vcoreDeiSetFsEvent(instObj, chObj, descMem);
    }

    return (retVal);
}
