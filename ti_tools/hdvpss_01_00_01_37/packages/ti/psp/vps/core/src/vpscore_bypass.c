/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_bypass.c
 *
 *  \brief VPS Bypass Path Core file.
 *  This file implements the core layer for 422 bypass paths - BP0 and BP1.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vpdmaMosaic.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/src/vpscore_bypassPriv.h>


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

static Vcore_Handle Vcore_bpOpen(UInt32 instId,
                                 Vcore_OpMode mode,
                                 UInt32 numCh,
                                 UInt32 perChCfg);
static Int32 Vcore_bpClose(Vcore_Handle handle);

static Int32 Vcore_bpGetProperty(Vcore_Handle handle,
                                 Vcore_Property *property);

static Int32 Vcore_bpSetFormat(Vcore_Handle handle,
                               UInt32 chNum,
                               const Vcore_Format *coreFmt);
static Int32 Vcore_bpGetFormat(Vcore_Handle handle,
                               UInt32 chNum,
                               Vcore_Format *coreFmt);

static Int32 Vcore_bpCreateLayout(Vcore_Handle handle,
                                  UInt32 chNum,
                                  const Vps_MultiWinParams *multiWinPrms,
                                  UInt32 *layoutId);
static Int32 Vcore_bpDeleteLayout(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId);

static Int32 Vcore_bpGetDescInfo(Vcore_Handle handle,
                                 UInt32 chNum,
                                 UInt32 layoutId,
                                 Vcore_DescInfo *descInfo);
static Int32 Vcore_bpSetFsEvent(Vcore_Handle handle,
                                UInt32 chNum,
                                VpsHal_VpdmaFSEvent fsEvent,
                                const Vcore_DescMem *descMem);

static Int32 Vcore_bpProgramDesc(Vcore_Handle handle,
                                 UInt32 chNum,
                                 UInt32 layoutId,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_bpProgramReg(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_DescMem *descMem);

static Int32 Vcore_bpUpdateDesc(Vcore_Handle handle,
                                const Vcore_DescMem *descMem,
                                const FVID2_Frame *frame,
                                UInt32 fid);
static Int32 Vcore_bpUpdateMultiDesc(Vcore_Handle handle,
                                     UInt32 layoutId,
                                     const Vcore_DescMem *descMem,
                                     const FVID2_FrameList *frameList,
                                     UInt32 fid,
                                     UInt32 startIdx,
                                     UInt32 *frmConsumed);
static Int32 Vcore_bpUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtPrms,
                                 UInt32 frmIdx);

static Int32 vcoreBpUpdateRtPrms(const Vcore_BpInstObj *instObj,
                                 Vcore_BpChObj *chObj,
                                 const Vcore_BpRtParams *rtPrms);
static Int32 vcoreBpUpdateRtDescMem(const Vcore_BpInstObj *instObj,
                                    Vcore_BpChObj *chObj,
                                    const Vcore_DescMem *descMem);

static Int32 Vcore_bpControl(Vcore_Handle handle, UInt32 cmd, Ptr args);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief Bypass path core function pointer table. */
static const Vcore_Ops VcoreBpCoreOps =
{
    Vcore_bpGetProperty,            /* Get property */
    Vcore_bpOpen,                   /* Open function */
    Vcore_bpClose,                  /* Close function */
    Vcore_bpSetFormat,              /* Set format */
    Vcore_bpGetFormat,              /* Get format */
    NULL,                           /* Set mosaic */
    Vcore_bpCreateLayout,           /* Create layout */
    Vcore_bpDeleteLayout,           /* Delete layout */
    Vcore_bpGetDescInfo,            /* Get descriptor info */
    Vcore_bpSetFsEvent,             /* Set frame */
    NULL,                           /* Set parameters */
    NULL,                           /* Get parameters */
    Vcore_bpControl,                /* Control */
    Vcore_bpProgramDesc,            /* Program descriptor */
    Vcore_bpProgramReg,             /* Program register */
    Vcore_bpUpdateDesc,             /* Update descriptor */
    Vcore_bpUpdateMultiDesc,        /* Update multiple window descriptor */
    NULL,                           /* Get DEI context info */
    NULL,                           /* Update context descriptor */
    Vcore_bpUpdateRtMem,            /* Update runtime params */
    NULL,                           /* Program scalar coefficient */
    NULL,                           /* Update the mode in DEI */
    NULL,                           /* Get scaling factor config */
};

/** \brief Properties of this core. */
static Vcore_Property VcoreBpProperty;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_bpInit
 *  \brief Bypass path core init function.
 *  Initializes BP core objects, allocates memory etc.
 *  This function should be called before calling any of BP core API's.
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
Int32 Vcore_bpInit(UInt32 numInst,
                   const Vcore_BpInitParams *initPrms,
                   Ptr arg)
{
    Int32               retVal = VPS_SOK;

    /* Check for errors */
    GT_assert(BpCoreTrace, (numInst <= VCORE_BP_INST_MAX));
    GT_assert(BpCoreTrace, (NULL != initPrms));

    /* Initialize the internal objects */
    retVal = vcoreBpPrivInit(numInst, initPrms);
    if (VPS_SOK == retVal)
    {
        /* Initialize bypass path core properties */
        VcoreBpProperty.numInstance = numInst;
        VcoreBpProperty.name = VCORE_TYPE_BP;
        VcoreBpProperty.type = VCORE_TYPE_INPUT;
        VcoreBpProperty.internalContext = FALSE;
    }

    /* Cleanup if error occurs */
    if (VPS_SOK != retVal)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Init Failed!!\n");
        Vcore_bpDeInit(NULL);
    }

    return (retVal);
}



/**
 *  Vcore_bpDeInit
 *  \brief Bypass path core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_bpDeInit(Ptr arg)
{
    Int32       retVal = VPS_SOK;

    retVal = vcoreBpPrivDeInit();
    VcoreBpProperty.numInstance = 0u;

    return (retVal);
}



/**
 *  Vcore_bpGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_bpGetCoreOps(void)
{
    return &VcoreBpCoreOps;
}



/**
 *  Vcore_bpOpen
 *  \brief Opens a particular instance of bypass path in either display
 *  or in M2M mode and returns the handle to the handle object.
 *  When opened in display mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in M2M mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_BP_MAX_HANDLES macro. Also each of the handles could have multiple
 *  channel to support more than one M2M operation is a single request.
 *
 *  \param instId       [IN] Instance to open - BP0 or BP1 instance.
 *  \param mode         [IN] Mode to open the instance for - display or M2M.
 *  \param numCh        [IN] Number of channel to be associated with this open.
 *                      For display operation only one channel is allowed.
 *                      For M2M operation, more than one channel could
 *                      be associated with a handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_BP_MAX_CHANNELS macro.
 *  \param perChCfg     [IN] Flag indicating whether the channel configuration
 *                      should be separate for each and every channel or
 *                      not. This is valid only for M2M drivers. For
 *                      display drivers this should be set to FALSE.
 *                      TRUE - Separate config descriptors for each channel.
 *                      FALSE - Same configuration for all the channels.
 *
 *  \return             If success returns the handle else returns NULL.
 */
static Vcore_Handle Vcore_bpOpen(UInt32 instId,
                                 Vcore_OpMode mode,
                                 UInt32 numCh,
                                 UInt32 perChCfg)
{
    Int32               retVal = VPS_SOK;
    Vcore_BpHandleObj  *hObj = NULL;

    /* Only display and M2M modes are supported */
    GT_assert(BpCoreTrace,
        ((VCORE_OPMODE_DISPLAY == mode) || (VCORE_OPMODE_MEMORY == mode)));
    /* Check for maximum channel supported per handle */
    GT_assert(BpCoreTrace, (numCh <= VCORE_BP_MAX_CHANNELS));
    /* There should be atleast one channel */
    GT_assert(BpCoreTrace, (0u != numCh));
    /* For display operation, only one channel should be allowed */
    GT_assert(BpCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (VCORE_MAX_DISPLAY_CH < numCh))));

    /* Allocate a handle object */
    hObj = vcoreBpAllocHandleObj(instId, mode);
    if (NULL != hObj)
    {
        /* Allocate channel objects */
        retVal = vcoreBpAllocChObjs(hObj, numCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Channel allocation failed!!\n");
            /* Free-up handle object if channel allocation failed */
            vcoreBpFreeHandleObj(hObj);
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
        GT_0trace(BpCoreTrace, GT_ERR, "Handle allocation failed!!\n");
    }

    return ((Vcore_Handle) hObj);
}



/**
 *  Vcore_bpClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       [IN] Bypass Path Core handle.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_bpClose(Vcore_Handle handle)
{
    Int32                   retVal = VPS_EFAIL;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Free channel and handle objects */
    retVal = vcoreBpFreeChObjs(hObj, hObj->numCh);
    retVal |= vcoreBpFreeHandleObj(hObj);
    if (VPS_SOK != retVal)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Free memory failed!!\n");
    }

    return (retVal);
}



/**
 *  Vcore_bpGetProperty
 *  \brief Gets the core properties of the bypass path core.
 *
 *  \param handle       [IN] Bypass Path Core handle.
 *  \param property     [OUT] Pointer to which the bypass path core properties
 *                      to be copied.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_bpGetProperty(Vcore_Handle handle,
                                 Vcore_Property *property)
{
    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != property));

    VpsUtils_memcpy(property, &VcoreBpProperty, sizeof(Vcore_Property));

    return (VPS_SOK);
}



/**
 *  Vcore_bpSetFormat
 *  \brief Sets the format for a given channel.
 *
 *  \param handle       [IN] Bypass Path Core handle.
 *  \param chNum        [IN] Channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
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
static Int32 Vcore_bpSetFormat(Vcore_Handle handle,
                               UInt32 chNum,
                               const Vcore_Format *coreFmt)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt, startCh, numCh, vpdmaChCnt;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != coreFmt));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the requested format is valid */
        retVal = vcoreBpCheckFmt(hObj->instObj, coreFmt);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Invalid Format!!\n");
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
            GT_assert(BpCoreTrace, (NULL != chObj));

            /* Copy the format to the local structure */
            VpsUtils_memcpy(&chObj->coreFmt, coreFmt, sizeof(Vcore_Format));

            /* Dummy data descriptor is not needed if frame size and buffer
             * bottom-right coordinates match */
            chObj->numExtraDesc = 0u;
            if (((coreFmt->startX + coreFmt->fmt.width)
                    < coreFmt->frameWidth) ||
                ((coreFmt->startY + coreFmt->fmt.height)
                    < coreFmt->frameHeight))
            {
                chObj->isDummyNeeded = TRUE;
                chObj->numExtraDesc += VCORE_BP_NUM_DESC_PER_WINDOW;
            }
            else
            {
                chObj->isDummyNeeded = FALSE;
                if (VCORE_OPMODE_DISPLAY == hObj->instObj->curMode)
                {
                    /* Always assume dummy is required for display mode as
                     * during runtime it might be needed when width/height or
                     * startX/startY changes. */
                    chObj->numExtraDesc += VCORE_BP_NUM_DESC_PER_WINDOW;
                }
            }

            /* Add data descriptor for SOCH and abort of actual channel
             * in display mode */
            if (VCORE_OPMODE_DISPLAY == hObj->instObj->curMode)
            {
                chObj->numExtraDesc += VCORE_BP_NUM_DESC_PER_WINDOW;
            }

            /*
             *  Set the channel information depending on format requested.
             *  Always supports YUV 422 interleaved data only.
             *  Supports no configuration and hence all overlay size is set
             *  to zero.
             */
            chObj->descInfo.numInDataDesc = VCORE_BP_NUM_DESC_PER_WINDOW;
            chObj->descInfo.numMultiWinDataDesc = chObj->numExtraDesc;
            chObj->descInfo.numChannels = hObj->instObj->numVpdmaCh;
            for(vpdmaChCnt = 0; vpdmaChCnt < hObj->instObj->numVpdmaCh;
                vpdmaChCnt++)
            {
                chObj->descInfo.socChNum[vpdmaChCnt] =
                    hObj->instObj->vpdmaCh[vpdmaChCnt];
            }

            /* Set proper state - format is set */
            chObj->state.isFormatSet = TRUE;
        }
    }

    return (retVal);
}



/**
 *  Vcore_bpGetFormat
 *  \brief Gets the format for a given channel.
 *
 *  \param handle       [IN] Bypass Path Core handle.
 *  \param chNum        [IN] Channel from which the format should be read.
 *                      When opened in display mode, this should be always set
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
static Int32 Vcore_bpGetFormat(Vcore_Handle handle,
                               UInt32 chNum,
                               Vcore_Format *coreFmt)
{
    Int32                   retVal = VPS_SOK;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != coreFmt));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        /* Copy the format */
        VpsUtils_memcpy(coreFmt, &chObj->coreFmt, sizeof(Vcore_Format));
    }

    return (retVal);
}



/**
 *  Vcore_bpCreateLayout
 *  \brief Creates a mosaic layout for a given channel. This allocates memory
 *  for storing the layout information and if successful then assigns a valid
 *  layout ID to layoutId parameter. This should be used for all future
 *  reference to the created layout.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
static Int32 Vcore_bpCreateLayout(Vcore_Handle handle,
                                  UInt32 chNum,
                                  const Vps_MultiWinParams *multiWinPrms,
                                  UInt32 *layoutId)
{
    Int32                   retVal = VPS_SOK;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != multiWinPrms));
    GT_assert(BpCoreTrace, (NULL != layoutId));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the requested mosaic config is valid */
        retVal = vcoreBpCheckMosaicCfg(multiWinPrms);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Invalid Mosaic Configuration!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        /* Format should be set first as the frame dimension used in mosaic
         * layout is set using set format function */
        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "Set the parameter before creating layouts!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Create the layout */
        retVal = vcoreBpCreateLayout(
                     hObj->instObj,
                     chObj,
                     multiWinPrms,
                     layoutId);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Create layout failed!!\n");
        }
    }

    return (retVal);
}



/**
 *  Vcore_bpDeleteLayout
 *  \brief Deletes the mosaic layout as identified by the layout ID.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
static Int32 Vcore_bpDeleteLayout(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId)
{
    Int32                   retVal = VPS_SOK;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        /* Delete the layout */
        retVal = vcoreBpDeleteLayout(hObj->instObj, chObj, layoutId);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Delete layout failed!!\n");
        }
    }

    return (retVal);
}



/**
 *  Vcore_bpGetDescInfo
 *  \brief Depending on the format set and the layout ID, returns the number of
 *  data descriptors and config overlay memory needed for the actual driver to
 *  allocate memory.
 *  Format should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
static Int32 Vcore_bpGetDescInfo(Vcore_Handle handle,
                                 UInt32 chNum,
                                 UInt32 layoutId,
                                 Vcore_DescInfo *descInfo)
{
    Int32                   retVal = VPS_SOK;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;
    Vcore_BpMosaicInfo     *mInfo;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != descInfo));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
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
            mInfo = vcoreBpGetLayoutObj(chObj, layoutId);
            if (NULL == mInfo)
            {
                GT_0trace(BpCoreTrace, GT_ERR, "Invalid layout ID!!\n");
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
 *  Vcore_bpSetFsEvent
 *  \brief Program the client's frame start.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
 *                      Note: In the current implementation only direct register
 *                      write is performed as the line mode is always the same.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_bpSetFsEvent(Vcore_Handle handle,
                                UInt32 chNum,
                                VpsHal_VpdmaFSEvent fsEvent,
                                const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_BpHandleObj      *hObj;
    VpsHal_VpdmaLineMode    lineMode = VPSHAL_VPDMA_LM_0;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    hObj = (Vcore_BpHandleObj *) handle;

    VpsHal_vpdmaSetFrameStartEvent(
        hObj->instObj->vpdmaCh[VCORE_BP_422P_IDX],
        fsEvent,
        lineMode,
        0u,
        NULL,
        0u);

    return (retVal);
}



/**
 *  Vcore_bpProgramDesc
 *  \brief Programs the data descriptor for a given channel depending on the
 *  layout ID passed.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
static Int32 Vcore_bpProgramDesc(Vcore_Handle handle,
                                 UInt32 chNum,
                                 UInt32 layoutId,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != descMem));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
        retVal = VPS_EOUT_OF_RANGE;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "Set the parameter before programming the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Program descriptors */
        if (VCORE_DEFAULT_LAYOUT_ID == layoutId)
        {
            retVal = vcoreBpProgramDesc(hObj->instObj, chObj, descMem);
        }
        else
        {
            retVal = vcoreBpProgramMosaicDesc(
                         hObj->instObj,
                         chObj,
                         layoutId,
                         descMem);
        }
    }

    return (retVal);
}



/**
 *  Vcore_bpProgramReg
 *  \brief Programs the registers for each of the HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
static Int32 Vcore_bpProgramReg(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_DescMem *descMem)
{
    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));

    /* Core does not support any register programming. Always return sucecss */
    return (VPS_SOK);
}



/**
 *  Vcore_bpUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       [IN] Bypass Path Core handle.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frame        [IN] Pointer to the FVID frame containing the buffer
 *                      address. This parameter should be non-NULL.
 *  \param fid          [IN] FID to be programmed in the descriptor.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_bpUpdateDesc(Vcore_Handle handle,
                                const Vcore_DescMem *descMem,
                                const FVID2_Frame *frame,
                                UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chNum;
    Void                   *descPtr;
    Vcore_BpChObj          *chObj;
    Vcore_BpInstObj        *instObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != descMem));
    GT_assert(BpCoreTrace, (NULL != frame));
    hObj = (Vcore_BpHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(BpCoreTrace, (NULL != instObj));

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
            GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Update data descriptor */
        descPtr = descMem->inDataDesc[0u];
        GT_assert(BpCoreTrace, (NULL != descPtr));
        VpsHal_vpdmaSetAddress(
            descPtr,
            fid,
            frame->addr[fid][FVID2_YUV_INT_ADDR_IDX]);
    }

    return (retVal);
}



/**
 *  Vcore_bpUpdateMultiDesc
 *  \brief Updates the multiple window buffer addresses provided in the frame
 *  list structure to the descriptor memories.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
static Int32 Vcore_bpUpdateMultiDesc(Vcore_Handle handle,
                                     UInt32 layoutId,
                                     const Vcore_DescMem *descMem,
                                     const FVID2_FrameList *frameList,
                                     UInt32 fid,
                                     UInt32 startIdx,
                                     UInt32 *frmConsumed)
{
    Int32                       retVal = VPS_SOK;
    Void                       *descPtr, *bufPtr;
    UInt32                      descOffset, bufOffset;
    UInt32                      winCnt;
    UInt32                      chNum;
    FVID2_Frame                *frame;
    Vcore_BpChObj              *chObj;
    Vcore_BpInstObj            *instObj;
    Vcore_BpHandleObj          *hObj;
    Vcore_BpMosaicInfo         *mInfo;
    const Vps_WinFormat        *winFmt;
    VpsHal_VpdmaMosaicWinFmt   *mulWinFmt;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != descMem));
    GT_assert(BpCoreTrace, (NULL != frameList));
    GT_assert(BpCoreTrace, (NULL != frmConsumed));

    hObj = (Vcore_BpHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(BpCoreTrace, (NULL != instObj));

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
                GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
                retVal = VPS_EOUT_OF_RANGE;
            }
        }
    }
    else
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Null Pointer!!\n");
        retVal = VPS_EBADARGS;
    }

    if (VPS_SOK == retVal)
    {
        chObj = hObj->chObjs[chNum];
        /* NULL pointer check */
        GT_assert(BpCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "Set the parameter before updating the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Check if layout ID is valid */
        if (VCORE_DEFAULT_LAYOUT_ID == layoutId)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "Update multiwin desc fn called in non-multi window mode!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Get the layout object for the requested layout */
        mInfo = vcoreBpGetLayoutObj(chObj, layoutId);
        if (NULL == mInfo)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Invalid layout ID!!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
        else
        {
            /* Check if numFrames is greater than required number of windows */
            if (frameList->numFrames <
                (startIdx + mInfo->appMulWinPrms.numWindows))
            {
                GT_0trace(BpCoreTrace, GT_ERR,
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
            if (winCnt < mInfo->mulWinPrms.numWindowsFirstRow)
            {
                /* Get descriptor pointer for the first row */
                descPtr = descMem->inDataDesc[descOffset];
            }
            else
            {
                if (winCnt == mInfo->mulWinPrms.numWindowsFirstRow)
                {
                    /* Reset the count when moving to 2nd row */
                    descOffset = 0u;
                }
                /* Get descriptor pointer for the non-first row */
                descPtr = descMem->multiWinDataDesc[descOffset];
            }
            GT_assert(BpCoreTrace, (NULL != descPtr));
            descOffset += VCORE_BP_NUM_DESC_PER_WINDOW;

            mulWinFmt = &mInfo->mulWinFmt[winCnt];
            winFmt = mulWinFmt->ipWindowFmt;
            GT_assert(BpCoreTrace, (NULL != winFmt));

            /* Get the frame index for this descriptor */
            GT_assert(BpCoreTrace,
                (mulWinFmt->frameIndex < mInfo->appMulWinPrms.numWindows));
            frame = frameList->frames[startIdx + mulWinFmt->frameIndex];
            if (NULL == frame)
            {
                GT_0trace(BpCoreTrace, GT_ERR, "Null Pointer!!\n");
                retVal = VPS_EBADARGS;
                break;
            }

            /* Calculate buffer address */
            bufPtr = frame->addr[fid][FVID2_YUV_INT_ADDR_IDX];
            if (NULL == bufPtr)
            {
                GT_0trace(BpCoreTrace, GT_ERR, "Null Pointer!!\n");
                retVal = VPS_EBADARGS;
                break;
            }

            /* Assuming 422 interleaved data - 2 bytes per pixel */
            bufOffset = (mulWinFmt->bufferOffsetX * 2u);
            bufOffset += (mulWinFmt->bufferOffsetY *
                            winFmt->pitch[FVID2_YUV_INT_ADDR_IDX]);

            /* Update data descriptor */
            bufPtr = (Void *) ((UInt32) bufPtr + bufOffset);
            VpsHal_vpdmaSetAddress(descPtr, fid, bufPtr);
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
 *  Vcore_bpUpdateRtMem
 *  \brief Updates the runtime height, width and pitch provided in rtPrms
 *  structure to the descriptor memories.
 *
 *  \param handle       [IN] Bypass Path Core handle.
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
 *  \param rtPrms       [IN] Pointer to the Vcore_BpRtParams structure
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
static Int32 Vcore_bpUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtPrms,
                                 UInt32 frmIdx)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt, startCh, numCh;
    Vcore_BpChObj          *chObj;
    Vcore_BpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));
    GT_assert(BpCoreTrace, (NULL != descMem));
    hObj = (Vcore_BpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid channel number!!\n");
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
            GT_assert(BpCoreTrace, (NULL != chObj));

            if (TRUE != chObj->state.isFormatSet)
            {
                GT_0trace(BpCoreTrace, GT_ERR,
                    "Set the format before setting the runtime parameters!!\n");
                retVal = VPS_EFAIL;
                break;
            }

            if (NULL != rtPrms)
            {
                retVal = vcoreBpUpdateRtPrms(hObj->instObj, chObj, rtPrms);
                if (VPS_SOK != retVal)
                {
                    GT_1trace(BpCoreTrace, GT_ERR,
                        "Update RT params failed for channel %d!!\n", chCnt);
                    break;
                }
            }

            retVal = vcoreBpUpdateRtDescMem(hObj->instObj, chObj, descMem);
            if (VPS_SOK != retVal)
            {
                GT_1trace(BpCoreTrace, GT_ERR,
                    "Update RT memory failed for channel %d!!\n", chCnt);
                break;
            }
        }
    }

    return (retVal);
}



/**
 *  vcoreBpUpdateRtPrms
 *  \brief Update runtime parameters into channel instance.
 */
static Int32 vcoreBpUpdateRtPrms(const Vcore_BpInstObj *instObj,
                                 Vcore_BpChObj *chObj,
                                 const Vcore_BpRtParams *rtPrms)
{
    Int32               retVal;
    UInt32              cnt;
    Vcore_Format       *coreFmt;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != rtPrms));
    coreFmt = &chObj->coreFmt;

    /* Copy the in frame params to core params */
    if (NULL != rtPrms->inFrmPrms)
    {
        coreFmt->fmt.width = rtPrms->inFrmPrms->width;
        coreFmt->fmt.height = rtPrms->inFrmPrms->height;
        /* For M2M mode, when dimension changes, change the frame dimensions as
         * well. This should not be done for display mode as frame width and
         * frame height represents display resolution and they never change. */
        if (VCORE_OPMODE_MEMORY == instObj->curMode)
        {
            coreFmt->frameWidth = rtPrms->inFrmPrms->width;
            coreFmt->frameHeight = rtPrms->inFrmPrms->height;
        }
        for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
        {
            coreFmt->fmt.pitch[cnt] = rtPrms->inFrmPrms->pitch[cnt];
        }
        coreFmt->memType = rtPrms->inFrmPrms->memType;
    }

    /* Copy the position params to core params */
    if (NULL != rtPrms->posCfg)
    {
        coreFmt->startX = rtPrms->posCfg->startX;
        coreFmt->startY = rtPrms->posCfg->startY;
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

    /* Check whether the updated RT params are valid */
    retVal = vcoreBpCheckFmt(instObj, coreFmt);
    if (VPS_SOK != retVal)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid runtime parameters!!\n");
    }

    return (retVal);
}



/**
 *  \brief Program vpdma inbound data descriptor based on channel instance
 *  parameters.
 */
static Int32 vcoreBpUpdateRtDescMem(const Vcore_BpInstObj *instObj,
                                    Vcore_BpChObj *chObj,
                                    const Vcore_DescMem *descMem)
{
    Int32       retVal;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != descMem));

    /* Re-program the descriptors */
    retVal = vcoreBpProgramDesc(instObj, chObj, descMem);

    return (retVal);
}

/**
 *  Vcore_bpControl
 *  \brief To handle control Command.
 *
 *  \param handle       BP Path Core handle.
 *  \param cmd          Control Command
 *  \param args         Command Specific Argument
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_bpControl(Vcore_Handle handle, UInt32 cmd, Ptr args)
{
    Int32                   retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != handle));

    switch (cmd)
    {
        default:
            GT_0trace(BpCoreTrace, GT_ERR, "Unsupported IOCTL\n");
            retVal = VPS_EUNSUPPORTED_CMD;
            break;
    }

    return (retVal);
}

