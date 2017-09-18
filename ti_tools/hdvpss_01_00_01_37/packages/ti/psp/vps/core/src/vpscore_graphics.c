/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_graphics.c
 *
 *  \brief VPS graphics Core file.
 *  This file implements the core layer for graphics paths
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
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_graphics.h>
#include <ti/psp/vps/core/src/vpscore_grpxPriv.h>
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

static Vcore_Handle Vcore_grpxOpen(UInt32 instId,
                                   Vcore_OpMode mode,
                                   UInt32 numCh,
                                   UInt32 perChCfg);
static Int32 Vcore_grpxClose(Vcore_Handle handle);

static Int32 Vcore_grpxGetProperty(Vcore_Handle handle,
                                   Vcore_Property *property);
static Int32 Vcore_grpxSetFormat(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_Format *coreFmt);
static Int32 Vcore_grpxGetFormat(Vcore_Handle handle,
                                 UInt32 chNUm,
                                 Vcore_Format *coreFmt);
static Int32 Vcore_grpxCreateLayout(Vcore_Handle handle,
                                    UInt32  chNum,
                                    const Vps_MultiWinParams *mRegs,
                                    UInt32 *layoutId);
static Int32 Vcore_grpxSetParams(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Void *arg);

static Int32 Vcore_grpxGetParams(Vcore_Handle handle,
                                 UInt32 chNum,
                                 Void *arg);

static Int32 Vcore_grpxGetDescInfo(Vcore_Handle handle,
                                   UInt32 chNum,
                                   UInt32 layoutId,
                                   Vcore_DescInfo *descInfo);
static Int32 Vcore_grpxProgramDesc(Vcore_Handle handle,
                                   UInt32 chNum,
                                   UInt32 layoutId,
                                   const Vcore_DescMem *descMem);
static Int32 Vcore_grpxProgramReg(Vcore_Handle handle,
                                  UInt32 chNum,
                                  const Vcore_DescMem *descMem);

static Int32 Vcore_grpxUpdateDesc(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  const FVID2_Frame *frame,
                                  UInt32 fid);

static Int32 Vcore_grpxUpdateMultiDesc(Vcore_Handle handle,
                                       UInt32 layoutId,
                                       const Vcore_DescMem *descMem,
                                       const FVID2_FrameList *frameList,
                                       UInt32 fid,
                                       UInt32 startIdx,
                                       UInt32 *frmConsumed);

static Int32 Vcore_grpxSetFsEvent(Vcore_Handle handle,
                                  UInt32 chNum,
                                  VpsHal_VpdmaFSEvent fsEvent,
                                  const Vcore_DescMem *descMem);

static Int32 Vcore_grpxUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtPrms,
                                 UInt32 regIdx);

static Int32 vcoreGrpxSetParams(Vcore_GrpxHandleObj *hObj,
                                Vcore_GrpxChObj *chObj,
                                Vps_GrpxRtParams *rtParams);

static Int32 vcoreGrpxProgramOvlyMem(Vcore_GrpxHandleObj *hObj,
                                     Vcore_GrpxChObj *chObj,
                                     const Vcore_DescMem *descMem);

static Int32 vcoreGrpxProgramDataDesc(Vcore_GrpxHandleObj *hObj,
                                      Vcore_GrpxChObj *chObj,
                                      const Vcore_DescMem *descMem);

static Int32 vcoreGrpxRtUpdateParams(Vcore_GrpxHandleObj *hObj,
                                     Vps_GrpxRtParams *rtParams,
                                     UInt32 chNum);

static Int32 vcoreGrpxRtProgramDataDesc(Vcore_GrpxHandleObj *hObj,
                                        Vcore_GrpxChObj *chObj,
                                        const Vcore_DescMem *descMem,
                                        UInt32 regId);


static Int32 vcoreGrpxRtProgramOvlyMem(Vcore_GrpxHandleObj *hObj,
                                       Vcore_GrpxChObj *chObj,
                                       const Vcore_DescMem *descMem,
                                       UInt32 regId);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** Core function pointer. */
static const Vcore_Ops GrpxCoreOps =
{
    Vcore_grpxGetProperty,            /* Get property */
    Vcore_grpxOpen,                   /* Open function */
    Vcore_grpxClose,                  /* Close function */
    Vcore_grpxSetFormat,              /* Set format */
    Vcore_grpxGetFormat,              /* Get format */
    NULL,                             /* Set mosaic */
    Vcore_grpxCreateLayout,           /* Create layout */
    NULL,                             /* Delete layout */
    Vcore_grpxGetDescInfo,            /* Get descriptor */
    Vcore_grpxSetFsEvent,             /* Set frame */
    Vcore_grpxSetParams,              /* set params */
    Vcore_grpxGetParams,              /* get params */
    NULL,                             /* control */
    Vcore_grpxProgramDesc,            /* program desc*/
    Vcore_grpxProgramReg,             /* program ovly*/
    Vcore_grpxUpdateDesc,             /* update desc */
    Vcore_grpxUpdateMultiDesc,        /* update multiple windows desc*/
    NULL,                             /* Get DEI context info */
    NULL,                             /* Update context descriptor */
    Vcore_grpxUpdateRtMem,            /* Update runtime params */
    NULL                              /* Program scalar coefficient */
};

/** Properties of this core */
static Vcore_Property         GrpxProperty;



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/**
 *  Vcore_grpxInit
 *  \brief Graphics Core init function.
 *  Initializes GRPX core objects, allocates memory etc.
 *  This function should be called before calling any of GRPX core API's.
 *
 *  \param numInst  [IN] Number of instance objects to be initialized
 *  \param initParams   [IN] Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances.
 *  \param arg          [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_grpxInit(UInt32 numInst,
                     const Vcore_GrpxInitParams *initParams,
                     Ptr arg)
{

    Int32                    retVal = VPS_SOK;

    retVal = vcoreGrpxPrivInit(numInst,initParams);
    if (VPS_SOK == retVal)
    {
        /* Initialize graphics path core properties */
        GrpxProperty.numInstance = numInst;
        GrpxProperty.name = VCORE_TYPE_GRPX;
        GrpxProperty.type = VCORE_TYPE_INPUT;
        GrpxProperty.internalContext = FALSE;
    }

    /* Cleanup if error occurs */
    if (VPS_SOK != retVal)
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,"GRPX Core Init Failed!!\n");
        Vcore_grpxDeInit(NULL);
    }

    return (retVal);
}
/*
 *  Vcore_grpxDeInit
 *  \brief Graphics Core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg         [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_grpxDeInit(Ptr arg)
{
    Int32         retVal = VPS_SOK;

    retVal = vcoreGrpxPrivDeinit();
    GrpxProperty.numInstance = 0u;
    return (retVal);
}
/**
 *  Vcore_grpxGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_grpxGetCoreOps(void)
{
    return &GrpxCoreOps;
}

/**
 *  Vcore_grpxOpen
 *  \brief Opens a particular instance of graphics in either display
 *  or in mem-mem mode and returns the handle to the handle object.  When
 *  opened in display mode, only one handle could be opened. Also only one
 *  channel is supported.  When opened in mem-mem mode, the same instance could
 *  be opened more than once. the number of handles supported per instance is
 *  determined by VCORE_GRPX_MAX_HANDLE macro. Also each of the handle could
 *  have multiple channel to support more than one mem-mem operation is a
 *  single submit.
 *
 *  \param instId       [IN] Instance to open - GRPX 0 -2.
 *  \param mode         [IN] mode to open the instance for - display or mem-mem.
 *  \param numCh   [IN] Number of channel to be associated with this open.
 *                      For display operation only one channel is allowed.  For
 *                      mem-mem operation, more than one channel could be
 *                      associated with a handle.  The maximum supported
 *                      channel per handle depends on VCORE_GRPX_MAX_CHANNELS
 *                      macro.
 *  \param perChCfg     [IN] Flag indicating whether the configure descriptors
 *                          should be separate for each and every channel or
 *                          not. This is valid only for mem-mem drivers. For
 *                          display drivers this should be set to FALSE.  TRUE
 *                          - Separate config descriptors for each channel.
 *                          FALSE - One config descriptors used per handle.
 *
 *  \return             If success returns the channel handle else returns NULL.
 */
static Vcore_Handle Vcore_grpxOpen(UInt32 instId,
                            Vcore_OpMode mode,
                            UInt32 numCh,
                            UInt32 perChCfg)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxHandleObj      *hObj = NULL;

    /* instance ID checking*/
    GT_assert(GrpxCoreTrace, (instId < VCORE_GRPX_NUM_INST));
    /* support only display and memory modes */
    GT_assert(GrpxCoreTrace,
            ((VCORE_OPMODE_DISPLAY == mode) || (VCORE_OPMODE_MEMORY == mode)));
    /* Check for maximum context supported per handle */
    GT_assert(GrpxCoreTrace, (numCh <= VCORE_GRPX_MAX_CHANNELS));
    /* at least one context when open*/
    GT_assert(GrpxCoreTrace, (0u != numCh));
    /*For display operation, only one channle should be allowed*/
    GT_assert(GrpxCoreTrace,
            (!((VCORE_OPMODE_DISPLAY == mode) &&
                    (VCORE_MAX_DISPLAY_CH < numCh))));
    /*For display operation, per channel format should be false*/
    GT_assert(GrpxCoreTrace,
        (!((VCORE_OPMODE_DISPLAY == mode) && (TRUE == perChCfg))));


    /* Allocate a handle object */
    hObj = vcoreGrpxAllocHandleObj(instId, mode);
    if (NULL != hObj)
    {
        /* Allocate channel objects */
        retVal = voreGrpxAllocChObj(hObj->chObjs, numCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                          "Channel allocation failed!!\n");
            /* Free-up channel object if context allocation failed */
            vcoreGrpxFreeHandleObj(hObj);
            hObj = NULL;
        }
        else
        {
            /* Initialize variables */
            hObj->perChCfg = perChCfg;
            hObj->numCh = numCh;
            if (FALSE == perChCfg)
                vcoreGrpxSetDefaultCh(&hObj->handleContext, 0u);
        }
    }
    else
    {
        GT_0trace(GrpxCoreTrace, GT_ERR, "Channel allocation failed!!\n");
    }

    return ((Vcore_Handle) hObj);
}

/**
 *  Vcore_grpxClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle      [IN] Graphics Core handle.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxClose(Vcore_Handle handle)
{
    Int32                    retVal = VPS_EFAIL;
    UInt32                   chCnt;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));

    hObj = (Vcore_GrpxHandleObj *) handle;

    /* Check whether the formatting is common for all context or
     * different for each of the context for a given channel. */
    if (TRUE == hObj->perChCfg)
    {
        for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            /* pointer sanity check */
            GT_assert(GrpxCoreTrace, (NULL != chObj));

            /* reset the flag*/
            if (TRUE == chObj->state.isMultiWinMode)
            {
                chObj->state.isMultiWinMode = FALSE;
            }

        }
    }
    else
    {
        /* Same format - so use channel context. Use each context
         * objects to just store the data descriptor information. */
        chObj = &hObj->handleContext;
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        /* Free the already allocated free channel */
        if (TRUE == chObj->state.isMultiWinMode)
        {
            chObj->state.isMultiWinMode = FALSE;
        }
    }
    /* Free context objects */
    retVal = vcoreGrpxFreeChObj(hObj->chObjs, hObj->numCh);
    /* Free channel objects */
    retVal |= vcoreGrpxFreeHandleObj(hObj);

    if (VPS_SOK != retVal)
    {
        GT_0trace(GrpxCoreTrace, GT_ERR, "Grpx Free Memory Failed!!\n");
    }

    return (retVal);
}

/**
 *  Vcore_grpxGetProperty
 *  \brief Gets the core properties of the graphics core.
 *
 *  \param handle     [IN] graphics handle
 *  \param property   [IN] Pointer to which the Graphics path core properties
 *                         to  be copied.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxGetProperty(Vcore_Handle handle,
                            Vcore_Property *property)
{
    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != property));

    VpsUtils_memcpy(property, &GrpxProperty, sizeof(Vcore_Property));

    return (VPS_SOK);
}

/**
 *  Vcore_grpxSetFormat
 *  \brief Sets the format for a given channel.
 *
 *  \param handle      [IN] Graphics Core  handle.
 *  \param chNum       [IN]  channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param coreFmt     [IN] Pointer to the format information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxSetFormat(Vcore_Handle handle,
                          UInt32 chNum,
                          const Vcore_Format *coreFmt)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj        *instObj;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;
    VpsHal_GrpxVpdmaOvlyInfo ovlyInfo;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_GrpxHandleObj *) handle;

    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));
    /* Check if the channel is within the range allocated during open */

    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR,"Invalid channel number!! \n");
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the requested format is valid */
        retVal = vcoreGrpxCheckFormat(hObj, coreFmt);
        if (VPS_SOK == retVal)
        {
            /* Check whether the formatting is common for all channel or
             * different for each of the channel for a given channel. */
            chObj = vcoreGrpxGetChannelObj(hObj, chNum);
            /* pointer sanity check */
            GT_assert(GrpxCoreTrace, (NULL != chObj));

            /*
             *  Set the channel information depending on format requested .
             */
            /*1 for clut 1 for stenciling, 1 for region .*/
            chObj->descInfo.numInDataDesc = 3u;
            chObj->descInfo.numMultiWinDataDesc = 0u;

            /*get the size of the overlay size for frame + coeff*/
            /*convert word to byte, for driver, it should */
            VpsHal_grpxGetOvlyInfo(instObj->grpxHandle,
                    VPSHAL_GRPX_VPDMA_OVLY_ALL_ATTRS, &ovlyInfo);

            chObj->descInfo.horzCoeffOvlySize = ovlyInfo.sizeOfOvly * 2u;
            chObj->descInfo.vertCoeffOvlySize = ovlyInfo.sizeOfOvly * 2u;
            chObj->descInfo.vertBilinearCoeffOvlySize = 0u;
            /*GRPX do not need this*/
            chObj->descInfo.numOutDataDesc = 0u;
            chObj->descInfo.shadowOvlySize = 0u;
            chObj->descInfo.nonShadowOvlySize = 0u;

            chObj->descInfo.coeffConfigDest = instObj->configDest;
            /*This are used to generate abort descriptor for main driver*/
            chObj->descInfo.numChannels = 3;
            chObj->descInfo.socChNum[0] = instObj->chClut;
            chObj->descInfo.socChNum[1] = instObj->chSten;
            chObj->descInfo.socChNum[2] = instObj->chGrpx;
            /* Copy the format to the local structure */
            VpsUtils_memcpy(
                    &chObj->coreFmt,
                    (Ptr) coreFmt,
                    sizeof(Vcore_Format));

            /* Set proper states - since format is set, desc info should be set
             * again */
            chObj->state.isFormatSet = TRUE;
        }
        else
        {
            GT_0trace(GrpxCoreTrace, GT_ERR, "Invalid Format!!\n");
        }
    }

    return (retVal);
}

/**
 *  Vcore_grpxGetFormat
 *  \brief Gets the format for a given channel.
 *
 *  \param handle      [IN] Graphics Core  handle.
 *  \param chNum       [IN] channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param coreFmt     [OUT] Pointer to the format information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */

static Int32 Vcore_grpxGetFormat(Vcore_Handle handle,
                                 UInt32 chNum,
                                 Vcore_Format *coreFmt)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_GrpxHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum < hObj->numCh)
    {
        /* Check whether the formatting is common for all channel or
         * different for each of the channel for a given channel. */
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        VpsUtils_memcpy((Ptr)coreFmt, &chObj->coreFmt, sizeof(Vcore_Format));
    }
    else
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR,"Invalid channel number!! \n");
    }
    return (retVal);
}

/**
 *  Vcore_grpxCreateLayout
 *  \brief create multi region mosaic layout
 *  \param handle      [IN] Graphics Core  handle.
 *  \param chNum       [IN] channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param mRegs      [OUT] Pointer to the Multiple Regions information.
 *                      This parameter should be non-NULL.
 *
 *  \param layoutId     [OUT] Pointer to the layout ID where the core ID of
 *                      created layout will be populated on success. Else this
 *                      will be set to VCORE_INVALID_LAYOUT_ID.
 *                      This parameter should be non-NULL.
 *  \return           Return VPS_SOK on success else return error value.
 */
static Int32 Vcore_grpxCreateLayout(Vcore_Handle handle,
                                    UInt32  chNum,
                                    const Vps_MultiWinParams *mRegs,
                                    UInt32 *layoutId)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   regCnt;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != mRegs));

    hObj = (Vcore_GrpxHandleObj *) handle;

    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR,"Invalid channel number!! \n");
    }
    if (VPS_SOK == retVal)
    {
        /* Check whether the formatting is common for all channel or
         * different for each of the channel for a given channel. */
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        retVal = vcoreGrpxCheckMultRegFormat(hObj, mRegs, &chObj->coreFmt);
        if ( 2u > mRegs->numWindows)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                "Number of Regions in this ioctl should not be small than 2\n");
            retVal = VPS_EINVALID_PARAMS;
        }
        /* Check if the number of rows is greater than supported. */
        if (VCORE_GRPX_MAX_REGIONS < mRegs->numWindows)
        {
            GT_1trace(GrpxCoreTrace, GT_ERR,
                  "Number of regions more than what could be supported (%d)\n",
                  VCORE_GRPX_MAX_REGIONS);
            retVal = VPS_EOUT_OF_RANGE;
        }

    }
    if (VPS_SOK == retVal)
    {
        chObj->mInfo.numRegions = mRegs->numWindows;

        for (regCnt = 0u; regCnt < mRegs->numWindows; regCnt++)
        {
            VpsUtils_memcpy(&chObj->mInfo.regFmt[regCnt],
                            &mRegs->winFmt[regCnt],
                            sizeof(Vps_WinFormat));
        }

        /* Set the state for mosaic display */
        chObj->state.isMultiWinMode= TRUE;
    }

    return (retVal);
}

/**
 *  Vcore_grpxSetParams.
 *  \brief Configures the Graphics core. Used to set the params for the graphics
 *
 *  \param handle      [IN] Graphics Path Core handle.
 *  \param chNum       [IN] channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param arg         [IN] Pointer to the parameters information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxSetParams(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Void * arg)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   numReg;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;
    Vps_GrpxRegionParams     *regParam;
    Vps_GrpxRtParams         *gParams;
    Vps_GrpxParamsList       *regList;

    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != arg));
    hObj = (Vcore_GrpxHandleObj *)handle;


    /* Check if the context is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR, "Invalid Channel Number !!\n");
    }

    if (VPS_SOK == retVal)
    {
        /* Check whether the formatting is common for all channel or
         * different for each of the channel for a given channel. */
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    "Set the format before setting the descriptors!!\n");
            retVal = VPS_EFAIL;

        }
        regList = (Vps_GrpxParamsList *)arg;
        if (0u == regList->numRegions)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                "number of regions in this ioctl can not be zero.\n");
            retVal = VPS_EINVALID_PARAMS;
        }
        else if (1u != regList->numRegions)
        {
            if (chObj->mInfo.numRegions != regList->numRegions)
            {
                GT_2trace(GrpxCoreTrace, GT_ERR,
                    " Region number %d in the params not match     \
                            number %d in MultRegion info.\n",
                    regList->numRegions, chObj->mInfo.numRegions);
                retVal = VPS_EINVALID_PARAMS;
            }
        }

    }
    if (VPS_SOK == retVal)
    {
        /*CLUT is always stored in the contextHandle since this is shared*/
        chObj->clutData = regList->clutPtr;
        /* store the coefficients*/
        if (NULL != regList->scParams)
        {
            VpsUtils_memcpy(&chObj->scParams,
                         regList->scParams,sizeof(Vps_GrpxScParams));
        }

        for (numReg = 0u; numReg < regList->numRegions; numReg++)
        {
            gParams = &regList->gParams[numReg];
            GT_assert(GrpxCoreTrace,(NULL != gParams));
            regParam = &gParams->regParams;
            if ((NULL == gParams->stenPtr) &&
                    (TRUE == regParam->stencilingEnable))
            {
                GT_0trace(GrpxCoreTrace,GT_ERR,
                        "stenciling enable without valid point!!\n");
                retVal = VPS_EBADARGS;
                break;
            }
            if ((NULL == regList->scParams) &&
                (TRUE == regParam->scEnable))
            {
                GT_0trace(GrpxCoreTrace,GT_ERR,
                        "Scaling enable without Coefficient!!\n");
                retVal = VPS_EBADARGS;
                break;

            }
            retVal = vcoreGrpxCheckPitchDataFormat(hObj, chObj,gParams);
            if (VPS_SOK == retVal)
            {
                retVal = vcoreGrpxSetParams(hObj, chObj,gParams);
            }

            if (VPS_SOK != retVal)
            {
                break;
            }
        }

        if (VPS_SOK == retVal)
        {
            chObj->state.isParamsSet = TRUE;
            /*first region will be in numInDataDes, totoal 3*/
            chObj->descInfo.numInDataDesc = VCORE_GRPX_INBOUND_DESC_PER_REGION;
            /* rest region will be in the multiwin desc*/
            chObj->descInfo.numMultiWinDataDesc = (regList->numRegions- 1u) *
                                        VCORE_GRPX_DESCS_PER_REGION;
            /* Set the state for mosaic display */
            if (regList->numRegions > 1u)
            {
                chObj->state.isMultiWinMode= TRUE;
            }
            else
            {
                chObj->state.isMultiWinMode = FALSE;
            }
        }
    }

    return (retVal);
}

/**
 *  Vcore_grpxGetParams.
 *  \brief Used to get the params for the graphics
 *
 *  \param handle     [IN]  Graphics Path Core Channel handle.
 *  \param chNum      [IN] Context to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      context to which this has to be applied. This
 *                      varies from 0 to the (number of context - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param arg        [OUT] Pointer to the parameters information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxGetParams(Vcore_Handle handle,
                                 UInt32 chNum,
                                 Void * arg)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   regCnt;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;
    Vps_GrpxParamsList       *regList;
    Vps_GrpxRtParams         *gParams;
    Vps_GrpxRegionParams     *regParams;
    void                     *tempCoeff;

    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != arg));


    hObj = (Vcore_GrpxHandleObj *)handle;
    regList = (Vps_GrpxParamsList  *)arg;

    /* Check if the context is within the range allocated during open */
    if (chNum < hObj->numCh)
    {

        /* Check whether the formatting is common for all channel or
         * different for each of the channel for a given channel. */
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    "Set the parameters first!!\n");
            return (VPS_EFAIL);
        }

        if (TRUE == chObj->state.isMultiWinMode)
        {
            regList->numRegions = chObj->mInfo.numRegions;
        }
        else
        {
            regList->numRegions = 1u;

        }
        for (regCnt = 0u; regCnt < regList->numRegions; regCnt++)
        {
            gParams = &regList->gParams[regCnt];
            gParams->regId = regCnt;
            if (TRUE == chObj->state.isMultiWinMode)
            {
                gParams->format = chObj->mInfo.regFmt[regCnt].dataFormat;
                gParams->pitch[FVID2_RGB_ADDR_IDX] =
                    chObj->mInfo.regFmt[regCnt].pitch[FVID2_RGB_ADDR_IDX];

                regParams = &chObj->mInfo.regParam[regCnt];
                gParams->stenPtr = chObj->mInfo.stenData[regCnt];
                gParams->stenPitch = chObj->mInfo.regFmt[regCnt].
                                        pitch[VCORE_GRPX_STEN_PITCH_INDEX];
            }
            else
            {
                gParams->format = chObj->coreFmt.fmt.dataFormat;
                gParams->pitch[FVID2_RGB_ADDR_IDX] =
                    chObj->coreFmt.fmt.pitch[FVID2_RGB_ADDR_IDX];

                regParams = &chObj->regParam;
                gParams->stenPtr = chObj->stenData;
                gParams->stenPitch = chObj->coreFmt.fmt.
                                    pitch[VCORE_GRPX_STEN_PITCH_INDEX];
            }
            VpsUtils_memcpy(&gParams->regParams,
                            regParams,
                            sizeof(Vps_GrpxRegionParams));
            gParams->scParams = NULL;
        }
        regList->clutPtr = chObj->clutData;
        /*do not change the coefficietns pointer*/
        tempCoeff = regList->scParams->scCoeff;
        /*Copy the scaling parameter exclude coefficients*/
        VpsUtils_memcpy(regList->scParams,
                            &chObj->scParams,
                            sizeof(Vps_GrpxScParams));
        regList->scParams->scCoeff = tempCoeff;

    }
    else
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR, "Invalid Channel Number !!\n");
    }


    return (retVal);
}

/**
 *  Vcore_grpxGetDescInfo
 *  \brief Depending on the format set, returns the number of data and config
 *  descriptors needed for the actual driver to allocate memory.
 *  Format should have be set for each of the context before calling this
 *  function.
 *
 *  \param handle      [IN] Graphics Core Channel handle.
 *  \param chNum       [IN] Channel to which the info is required.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      context to which this has to be applied. This
 *                      varies from 0 to the (number of context - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param layoutId     [IN] Layout ID for which to return the information.
 *  \param descInfo   [OUT] Pointer to the data and config descriptor
 *                      information.  This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_grpxGetDescInfo(Vcore_Handle handle,
                            UInt32 chNum,
                            UInt32 layoutId,
                            Vcore_DescInfo *descInfo)
{
    Int32                    retVal = VPS_EFAIL;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != descInfo));

    hObj = (Vcore_GrpxHandleObj *) handle;

    /* Check if the context is within the range allocated during open */
    if (chNum < hObj->numCh)
    {
        /* Check whether the formatting is common for all channel or
         * different for each of the channel for a given channel. */
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE == chObj->state.isParamsSet)
        {
            retVal = VPS_SOK;
            /* Copy the data descriptor information */
            VpsUtils_memcpy(
                    (Ptr) descInfo,
                    &chObj->descInfo,
                    sizeof(Vcore_DescInfo));
        }
    }
    else
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR,"Invalid channel number!! \n");
    }

    return (retVal);
}


/**
 *  Vcore_grpxProgramDesc
 *  \brief Programs the data descriptor for a given channel.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       [IN] Grpx Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param layoutId     [IN] Layout ID used to program the descriptor.
 *  \param descMem      [IN] Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */

static Int32 Vcore_grpxProgramDesc(Vcore_Handle handle,
                                   UInt32 chNum,
                                   UInt32 layoutId,
                                   const Vcore_DescMem *descMem)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != descMem));

    hObj = (Vcore_GrpxHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    "Set the parameters before setting the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Set Data descriptors */
        retVal = vcoreGrpxProgramDataDesc(hObj, chObj, descMem);
    }

    return (retVal);
}

/**
 *  Vcore_grpxProgramReg
 *  \brief Programs the overlay memory for a given channel.
 *
 *  \param handle       [IN] Graphics Core handle.
 *  \param chNum        [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param descMem      [IN] Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actual registers. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxProgramReg(Vcore_Handle handle,
                                  UInt32 chNum,
                                  const Vcore_DescMem *descMem)
{
    Int32                    retVal = VPS_EFAIL;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != descMem));

    hObj = (Vcore_GrpxHandleObj *) handle;

    /* Check if the context is within the range allocated during open */
    if (chNum < hObj->numCh)
    {
        retVal = VPS_SOK;
        /* Check whether the formatting is common for all channel or
         * different for each of the channel for a given channel. */
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    "Set the parameters before setting the descriptors!!\n");
            retVal = VPS_EFAIL;
        }
    }
    else
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace,GT_ERR,"Invalid channel number!! \n");
    }

    if (VPS_SOK == retVal)
    {
        /* Set proper states - We are ready to transfer data!! */
        retVal = vcoreGrpxProgramOvlyMem(hObj,chObj,descMem);
    }
    return (retVal);
}

/**
 *  Vcore_grpxUpdateDesc
 *  \brief Update the descriptor information
 *
 *  \param handle  [IN] Graphics handle.
 *  \param descMem [IN] Pointer to the data descriptor memory information.
 *                     This parameter should be non-NULL.
 *  \param frame   [IN]Pointer to the FVID2 frame containing the buffer address.
 *                      This parameter should be non-NULL.
 *  \param fid     [IN]  FID to be programmed in the descriptor.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */

static Int32 Vcore_grpxUpdateDesc(Vcore_Handle handle,
                                  const Vcore_DescMem *descMem,
                                  const FVID2_Frame *frame,
                                  UInt32 fid)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   descNum;
    UInt32                   chNum;
    Ptr                      dataDesc;
    Vcore_GrpxInstObj        *instObj;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /*sanity checking*/
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != frame));
    GT_assert(GrpxCoreTrace, (NULL != descMem));

    hObj = (Vcore_GrpxHandleObj *)handle;
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    /* ignore the channle number for display */
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        chNum = 0u;
    }
    else
    {
        chNum = frame->channelNum;
        /*channel number checking*/
        if (chNum >= hObj->numCh)
        {
            retVal = VPS_EOUT_OF_RANGE;
            GT_0trace(GrpxCoreTrace, GT_ERR,
                     "Invalid channel number!!\n");

        }
    }
    if (FVID2_SOK == retVal)
    {
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        GT_assert(chObj, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    " Set the parameters before updating!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
        if (TRUE == chObj->state.isMultiWinMode)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    "Update non-region based desc fn in region-based mode!!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    /*Update the buffer address*/
    if (VPS_SOK == retVal)
    {
        /*This is for stenciling data Set the descriptor address */
        for (descNum = 0u; descNum < instObj->numDescPerReg; descNum++)
        {
            dataDesc =(Ptr)descMem->inDataDesc[descNum + 1u];
            if (descNum == 0u)
            {
                if (TRUE == chObj->regParam.stencilingEnable)
                {
                    VpsHal_vpdmaSetAddress(dataDesc,fid,chObj->stenData);
                }
            }
            else
            {
                VpsHal_vpdmaSetAddress(dataDesc, fid,
                    frame->addr[fid][FVID2_RGB_ADDR_IDX]);

                GT_1trace(GrpxCoreTrace, GT_DEBUG, "Buffer update  0x%0.8x\n",
                            frame->addr[fid][FVID2_RGB_ADDR_IDX]);

            }
        }
    }
    return (retVal);
}

/**
 *  Vcore_grpxUpdateMultiDesc
 *  \brief Updates the multiple region buffer addresses provided in the frame
 *  list structure to the descriptor memories.
 *
 *  \param handle       GRPX Path Core handle.
 *  \param layoutId     Layout ID to be used to update the descriptor memory.
 *  \param descMem      Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frameList    Pointer to the FVID frame containing the buffer address.
 *                      This parameter should be non-NULL.
 *  \param fid          FID to be programmed in the descriptor.
 *  \param startIdx     Index to the frame list from where the core will start
 *                      using the frames for updating multi window descriptors.
 *  \param frmConsumed  Number of frames consumed from frame list.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxUpdateMultiDesc(Vcore_Handle handle,
                                       UInt32 layoutId,
                                       const Vcore_DescMem *descMem,
                                       const FVID2_FrameList *frameList,
                                       UInt32 fid,
                                       UInt32 startIdx,
                                       UInt32 *frmConsumed)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   descNum;
    UInt32                   chNum;
    UInt32                   regionNum;
    Vcore_GrpxInstObj        *instObj;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;
    FVID2_Frame              *frame;
    Ptr                      dataDesc;

    /* sanity check*/
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != frameList));
    GT_assert(GrpxCoreTrace, (NULL != descMem));
    GT_assert(GrpxCoreTrace, (NULL != frmConsumed));

    hObj = (Vcore_GrpxHandleObj *)handle;
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    if (NULL != frameList->frames[startIdx])
    {
        /* ignore the channle number for display */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            chNum = 0u;
        }
        else
        {
            chNum = frameList->frames[startIdx]->channelNum;
            /* Check if the channel is within the range allocated during open */
            if (chNum >= hObj->numCh)
            {
                retVal = VPS_EOUT_OF_RANGE;
                GT_0trace(GrpxCoreTrace, GT_ERR, "Invalid channel number!!\n");
            }
        }
    }
    else
    {
        GT_0trace(GrpxCoreTrace, GT_ERR, "Null Pointer!!\n");
        retVal = VPS_EBADARGS;
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreGrpxGetChannelObj(hObj,chNum);
        GT_assert(chObj, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    " Set the parameters before updating!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Check if mosaic mode flag is set */
        if (TRUE != chObj->state.isMultiWinMode)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
             "Update region-based desc fn called in non-region based mode!!\n");
            retVal = VPS_EINVALID_PARAMS;
        }

        /* Check if the numFrames is greater than required number of windows */
        if (frameList->numFrames < (startIdx + chObj->mInfo.numRegions))
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                "Num frames not sufficient for required multi window mode!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        for (regionNum = startIdx; regionNum <
                        (startIdx + chObj->mInfo.numRegions); regionNum++)
        {
            frame = frameList->frames[regionNum];
            if (NULL == frame)
            {
                GT_0trace(GrpxCoreTrace, GT_ERR, "Null Pointer!!\n");
                retVal = VPS_EBADARGS;
                break;
            }

            if (VPS_SOK == retVal)
            {
                for (descNum = 0u; descNum < instObj->numDescPerReg &&
                            VPS_SOK == retVal; descNum++)
                {
                    /*get the right descirptor memory based the region num*/
                    if (0u == regionNum)
                    {
                        dataDesc = descMem->inDataDesc[1u + descNum];
                    }
                    else
                    {
                        dataDesc = descMem->
                            multiWinDataDesc[VCORE_GRPX_DESCS_PER_REGION *
                                  (regionNum - 1) + descNum];
                    }

                    if (descNum == 0u)
                    {
                        if (TRUE == chObj->mInfo.regParam[regionNum].
                                                             stencilingEnable)
                        {
                            VpsHal_vpdmaSetAddress(dataDesc,fid,
                                       chObj->mInfo.stenData[regionNum]);
                        }
                    }
                    else
                    {
                        VpsHal_vpdmaSetAddress(dataDesc, fid,
                            frame->addr[fid][FVID2_RGB_ADDR_IDX]);

                        GT_1trace(GrpxCoreTrace, GT_DEBUG,
                                    "Buffer update  0x%0.8x\n",
                                    frame->addr[fid][FVID2_RGB_ADDR_IDX]);

                    }
                }
            }
             (*frmConsumed)++;
        }
    }
    return retVal;
}

/**
 *  Vcore_grpxSetFsEvent
 *  \brief Program the client's frame start.
 *
 *  \param handle      [IN] Graphics handle.

 *  \param chNum       [IN] Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param fsEvent    [IN]  Frame start event.
 *  \param descMem    [IN]  Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actual registers. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */

static Int32 Vcore_grpxSetFsEvent(Vcore_Handle handle,
                                  UInt32 chNum,
                                  VpsHal_VpdmaFSEvent fsEvent,
                                  const Vcore_DescMem *descMem)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj        *instObj;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != handle));

    hObj = (Vcore_GrpxHandleObj *) handle;
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));


    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        /* pointer sanity check */
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    "Set the Params before setting the frame start event!!\n");
            retVal = VPS_EFAIL;
        }
    }

    if (VPS_SOK == retVal)
    {

        /*FSEVENT for stenciling is channel active(7), no LM FID*/
        VpsHal_vpdmaSetFrameStartEvent(
                instObj->chSten,
                VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                VPSHAL_VPDMA_LM_0,
                0u,
                NULL,
                0u);
        /*graphcis data channel FSEVENT is toggle LM filed 0*/
        VpsHal_vpdmaSetFrameStartEvent(
                instObj->chGrpx,
                fsEvent,
                VPSHAL_VPDMA_LM_0,
                0u,
                NULL,
                0u);
    }
    return (retVal);
}



/**
 *  Vcore_grpxUpdateRtMem
 *  \brief Updates the runtime parameters in rtPrms
 *  structure to the descriptor memories.
 *
 *  \param handle       [IN] GRPX Core handle.
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
 *  \param rtPrms       [IN] Per frame runtime configuration structure pointer.
 *                      This parameter could be NULL(Vcore_GrpxRtParams), if
 *                      it was NULL, then core will update regions information
 *                      based on previous settings
 *
 *  \frmIdx             [IN] which region will be update by this function.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
static Int32 Vcore_grpxUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtPrms,
                                 UInt32 regIdx)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   regNum;
    Vcore_GrpxHandleObj      *hObj;
    Vcore_GrpxChObj          *chObj;
    Vcore_GrpxRtParams       *grpxRtParams;
    Vps_GrpxScParams         oldscParams;
    GT_assert(GrpxCoreTrace, (NULL != handle));
    GT_assert(GrpxCoreTrace, (NULL != descMem));
    hObj = (Vcore_GrpxHandleObj *)handle;

    /*channel number checking*/
    if ( chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(GrpxCoreTrace, GT_ERR,
                 "Invalid channel number!!\n");

    }
    else
    {
        chObj = vcoreGrpxGetChannelObj(hObj, chNum);
        GT_assert(GrpxCoreTrace, (NULL != chObj));

        if (TRUE != chObj->state.isParamsSet)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                    " Set the parameters before updating!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
    }


    if (VPS_SOK == retVal)
    {
        if (TRUE == chObj->state.isMultiWinMode)
        {
            regNum = chObj->mInfo.numRegions;
        }
        else
        {
            regNum = 1u;
        }

        /*ID checking*/
        if (regIdx >= regNum)
        {
            GT_2trace(GrpxCoreTrace, GT_ERR,
                "region ID %d is out of range %d\n", regIdx, regNum);
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {

        grpxRtParams = (Vcore_GrpxRtParams *)rtPrms;

        if (NULL != grpxRtParams)
        {
            /*update the local copy*/
            if (VPS_SOK == retVal)
            {
                if (grpxRtParams->rtList)
                {
                    /*update the CLUT and coefficients*/
                    if (grpxRtParams->rtList->clutPtr)
                    {
                        vcoreGrpxCreateClutDataDesc(hObj,
                                                    descMem->inDataDesc[0],
                                                    grpxRtParams->rtList->clutPtr);
                        chObj->clutData = grpxRtParams->rtList->clutPtr;
                    }
                    /*update the scaling coefficients*/
                    if (grpxRtParams->rtList->scParams)
                    {
                        VpsUtils_memcpy(&oldscParams,
                                    &chObj->scParams, sizeof(Vps_GrpxScParams));
                        VpsUtils_memcpy(&chObj->scParams,
                                       grpxRtParams->rtList->scParams,sizeof(Vps_GrpxScParams));

                    }
                }
                /*update region parameters*/
                if (VPS_SOK == retVal)
                {
                    if (grpxRtParams->rtParams)
                    {
                        /*update the runtime parameter into local copy*/
                        retVal = vcoreGrpxRtUpdateParams(hObj,
                                                     grpxRtParams->rtParams,
                                                     chNum);
                    }
                    if (VPS_SOK != retVal)
                    {
                        /*not update the sc if any errors*/
                        VpsUtils_memcpy(&chObj->scParams,
                                    &oldscParams, sizeof(Vps_GrpxScParams));

                    }
                    /*update the scaling information*/
                    if ((VPS_SOK == retVal) && (grpxRtParams->rtList) &&
                            (grpxRtParams->rtList->scParams))
                    {
                            retVal = vcoreGrpxRtProgramOvlyMem(hObj,
                                                               chObj,
                                                               descMem,
                                                               regIdx);
                    }
                }
            }
        }
    }
    if (VPS_SOK == retVal)
    {
        /*program the data descriptor if applicable*/
        retVal = vcoreGrpxRtProgramDataDesc(hObj,
                                            chObj,
                                            descMem,
                                            regIdx);
    }
    else
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,
            "Core update RT params failed.\n ");
    }

    return (retVal);
}

/**
 *  vcoreGrpxSetParams
 *  this function is to set the params into core driver's local structure
 *  returns VPS_SOK on success else return error value
 */
static Int32 vcoreGrpxSetParams(Vcore_GrpxHandleObj *hObj,
                                Vcore_GrpxChObj *chObj,
                                Vps_GrpxRtParams *rtParams)
{

    Int32                    retVal = VPS_SOK;
    UInt32                   regId;
    UInt32                   pitchCnt;
    UInt32                   numReg;
    Vcore_GrpxInstObj        *instObj;
    VpsHal_GrpxFrmDispAttr   frmAttr;
    Vps_GrpxRegionParams     *prevParams;
    Vps_GrpxRegionParams     *regParams;
    Vps_GrpxScParams         *scParams;
    UInt32                   prevYOffset = 0;

    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));
    GT_assert(GrpxCoreTrace, (NULL != rtParams));

    regId = rtParams->regId;
    regParams = &rtParams->regParams;

    if (TRUE == chObj->state.isMultiWinMode)
    {
        prevParams = &chObj->mInfo.regParam[regId];
        if (NULL != rtParams->stenPtr)
        {
            chObj->mInfo.stenData[regId] = rtParams->stenPtr;
        }
        numReg = chObj->mInfo.numRegions;
    }
    else
    {
        prevParams = &chObj->regParam;
        if (NULL != rtParams->stenPtr)
        {
            chObj->stenData = rtParams->stenPtr;
        }
        numReg = 1u;
    }



    /*compare the current reg parameters with the old one
     only update if the current is not the same as the old one*/
    if (0u != VpsUtils_memcmp((const Void *)regParams,
            (const Void *)prevParams, sizeof(Vps_GrpxRegionParams)))
    {


        /*get the frame attributes*/
        retVal = vcoreGrpxGetFrmAttr(chObj,&frmAttr);


        GT_assert(GrpxCoreTrace, (((0u == regId) &&
                   (TRUE == regParams->firstRegion)) ||
                   ((0u != regId) && (FALSE == regParams->firstRegion))));

        GT_assert(GrpxCoreTrace,((((numReg - 1u) == regId) &&
                    (TRUE == regParams->lastRegion)) ||
                    (((numReg - 1u) != regId) &&
                        (FALSE == regParams->lastRegion))));


        if (NULL != rtParams->scParams)
        {
            scParams = rtParams->scParams;
        }
        else
        {
            scParams = &chObj->scParams;
        }


        if ((TRUE == regParams->firstRegion) && (regId == 0u))
        {
            prevYOffset = 0;
        }

        else
        {
            /*get the end y position of previous region to make sure
             that the next regio does not overlay on it*/
             if (regId > 0u) {
                prevYOffset = VpsHal_grpxGetRegionYEnd(
                                        &chObj->mInfo.regParam[regId - 1],
                                        scParams);
             }

        }

        retVal = VpsHal_grpxRegionParamsCheck(
                    instObj->grpxHandle,
                    &frmAttr,
                    regParams,
                    scParams,
                    prevYOffset);
        if (VPS_SOK == retVal)
        {
            /*Figure out do we need update the stenciling descriptor*/
            if (TRUE == chObj->state.isMultiWinMode)
            {
                chObj->mInfo.regFmt[regId].winStartX = regParams->regionPosX;
                chObj->mInfo.regFmt[regId].winStartY = regParams->regionPosY;

                chObj->mInfo.regFmt[regId].winWidth = regParams->regionWidth;
                chObj->mInfo.regFmt[regId].winHeight = regParams->regionHeight;
            }
            else
            {
                chObj->coreFmt.startX = regParams->regionPosX;
                chObj->coreFmt.startY = regParams->regionPosY;

                chObj->coreFmt.fmt.width = regParams->regionWidth;
                chObj->coreFmt.fmt.height = regParams->regionHeight;
            }

            /*update the region attributes*/
            if (TRUE == chObj->state.isMultiWinMode)
            {
                VpsUtils_memcpy(&chObj->mInfo.regParam[regId],
                                regParams,
                                sizeof(chObj->regParam));
            }
            else
            {
                VpsUtils_memcpy(&chObj->regParam,
                                regParams,
                                sizeof(chObj->regParam));
            }
        }
        else
        {
            GT_1trace(GrpxCoreTrace, GT_ERR,
                " Region%d Params Check Failed.\n",regId);

            if(TRUE == regParams->scEnable)
            {
                GT_4trace(GrpxCoreTrace, GT_ERR,
                    "output Region %d x %d at %d x %d\n",
                     scParams->outWidth,scParams->outHeight,
                    regParams->regionPosX,regParams->regionPosY);
            }
            else
            {
                GT_4trace(GrpxCoreTrace, GT_ERR,
                    "output Region %d x %d at %d x %d\n",
                     regParams->regionWidth, regParams->regionHeight,
                     regParams->regionPosX,regParams->regionPosY);

            }
        }
    }


    /*update the rest parameters*/
    if (VPS_SOK == retVal)
    {
        if (TRUE == chObj->state.isMultiWinMode)
        {
            chObj->mInfo.regFmt[regId].dataFormat = rtParams->format;
            chObj->mInfo.regFmt[regId].bpp =
                            vcoreGrpxGetFvid2Bpp(rtParams->format);
            for (pitchCnt = 0u; pitchCnt < FVID2_MAX_PLANES; pitchCnt++)
            {
                chObj->mInfo.regFmt[regId].pitch[pitchCnt] =
                                            rtParams->pitch[pitchCnt];
            }
            /* use the pitch[2] to store the stenciling stride*/
            chObj->mInfo.regFmt[regId].pitch[VCORE_GRPX_STEN_PITCH_INDEX] =
                                    rtParams->stenPitch;
        }
        else
        {

            chObj->coreFmt.fmt.dataFormat= rtParams->format;
            chObj->coreFmt.fmt.bpp = vcoreGrpxGetFvid2Bpp(rtParams->format);
            for (pitchCnt = 0u; pitchCnt < FVID2_MAX_PLANES; pitchCnt++)
            {
                chObj->coreFmt.fmt.pitch[pitchCnt] = rtParams->pitch[pitchCnt];
            }
            /* use the pitch[2] to store the stenciling stride*/
            chObj->coreFmt.fmt.pitch[VCORE_GRPX_STEN_PITCH_INDEX] =
                                    rtParams->stenPitch;
        }
        /* update the sc only if it is a brand new settings*/
        if ((NULL != rtParams->scParams) &&
                    (0u != VpsUtils_memcmp(rtParams->scParams,
                        &chObj->scParams,sizeof(Vps_GrpxScParams))))
        {
            VpsUtils_memcpy(&chObj->scParams,
                            rtParams->scParams,
                            sizeof(Vps_GrpxScParams));
        }
    }
    return (retVal);
}

/**
 * vcoreGrpxProgramOvlyMem
 *  program the overlay memory
 *  return VPS_SOK on success else return error value
 */
static Int32 vcoreGrpxProgramOvlyMem(Vcore_GrpxHandleObj *hObj,
                                     Vcore_GrpxChObj *chObj,
                                     const Vcore_DescMem *descMem)
{
    Int32                    retVal = VPS_EFAIL;
    UInt32                   *tempPtr;
    UInt32                   wordCnt;
    Vcore_GrpxInstObj        *instObj;
    VpsHal_GrpxFrmDispAttr   frmAttr;

    /*pointer sanity check*/
    GT_assert(GrpxCoreTrace, (NULL != hObj));
    GT_assert(GrpxCoreTrace ,(NULL != chObj));
    GT_assert(GrpxCoreTrace, (NULL != descMem));
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    retVal = vcoreGrpxGetFrmAttr(chObj, &frmAttr);
    if (VPS_SOK != retVal)
    {
        GT_0trace(GrpxCoreTrace, GT_ERR, "Frame attribute get failed.!!\n");
        return (VPS_EFAIL);

    }
    /*set the frame attribute*/
    retVal = VpsHal_grpxSetFrameAttr(instObj->grpxHandle, &frmAttr, FALSE,
            descMem->coeffOvlyMem);

    /* Print the created descriptor memory. */
    GT_1trace(GrpxCoreTrace, GT_DEBUG,
             "----Frame Attribute Config 0x%0.8x----\n",descMem->coeffOvlyMem);

    tempPtr = (UInt32 *)descMem->coeffOvlyMem;
    for (wordCnt=0u; wordCnt < 4u; wordCnt++)
    {
        GT_2trace(GrpxCoreTrace,GT_DEBUG,
                      "Word %d Content 0x%0.8x\n",wordCnt, *tempPtr++);
    }

    if (VPS_SOK != retVal)
    {
        GT_0trace(GrpxCoreTrace, GT_ERR, "VPDMA Descriptor failed.!!\n");
        return (VPS_EFAIL);
    }


    if (VPS_SOK == retVal)
    {
        retVal = vcoreGrpxRtProgramOvlyMem(hObj,
                                           chObj,
                                           descMem,
                                           0u);

    }

    return (retVal);
}

/**
 *  vcoreGrpxProgramDataDesc
 *  Programs the data descriptor depending on the parameters set.
 *  This is used in case of non-mosaic mode of operation.
 *  return VPS_SOK on success else return error value
 */

static Int32 vcoreGrpxProgramDataDesc(Vcore_GrpxHandleObj *hObj,
                                  Vcore_GrpxChObj *chObj,
                                  const Vcore_DescMem *descMem)
{
    Int32                    retVal = VPS_SOK;
    UInt32                   numReg;
    UInt32                   regCnt;
    Ptr                      descPtr;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != hObj));
    GT_assert(GrpxCoreTrace, (NULL != chObj));
    GT_assert(GrpxCoreTrace, (NULL != descMem));

    if (FALSE == chObj->state.isMultiWinMode)
    {
        regCnt = 1u;
    }
    else
    {
        regCnt = chObj->mInfo.numRegions;
    }

    /*  Handle CLUT table first
     *  data descriptor for the CLUT channel is always in the inbound
     *  descriptor and it is alway the first one
     */
    descPtr = descMem->inDataDesc[0u];
    if (NULL != chObj->clutData)
    {
        vcoreGrpxCreateClutDataDesc(hObj, descPtr,chObj->clutData);
    }
    else
    {
        /*create dummy descriptor for the CLUT if CLUT is not exist*/
        VpsHal_vpdmaCreateDummyDesc(descPtr);
        descPtr =(Ptr)((UInt32)descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE);
        VpsHal_vpdmaCreateDummyDesc(descPtr);
    }

    /* This is for the stenciling data and GRPX data descriptors
     * each region has two descriptor, the first is stenciling and
     * second is real GRPX data*/
    for (numReg = 0u; numReg < regCnt; numReg++)
    {
        /*update each region alone*/
        retVal = vcoreGrpxRtProgramDataDesc(hObj,chObj, descMem, numReg);

        if (VPS_SOK != retVal)
        {
            break;
        }
    }
    return (retVal);
}


/**
 *  vcoreGrpxRtUpdateParams
 *  update the runtime parameters
 *  return VPS_SOK on success else return error value
 */

static Int32 vcoreGrpxRtUpdateParams(Vcore_GrpxHandleObj *hObj,
                                     Vps_GrpxRtParams *rtParams,
                                     UInt32 chNum)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxChObj          *chObj;

    GT_assert(GrpxCoreTrace,(NULL != hObj));
    chObj = vcoreGrpxGetChannelObj(hObj, chNum);
    GT_assert(GrpxCoreTrace,(NULL != chObj));
    if (NULL != rtParams)
    {
        retVal = vcoreGrpxCheckPitchDataFormat(hObj, chObj,rtParams);
        if (VPS_SOK == retVal )
        {
            retVal= vcoreGrpxSetParams(hObj,chObj,rtParams);
        }
    }
    return (retVal);
}

/**
 *  vcoreGrpxRtProgramDataDesc
 *  runtime program the data descriptor
 *  return VPS_SOK on success else return error value
 */
static Int32 vcoreGrpxRtProgramDataDesc(Vcore_GrpxHandleObj *hObj,
                                        Vcore_GrpxChObj *chObj,
                                        const Vcore_DescMem *descMem,
                                        UInt32 regId)

{
    Int32                    retVal = VPS_SOK;
    UInt32                   descSet;
    Ptr                      dataDesc;
    Vps_GrpxRegionParams     *regParams;
    Vcore_GrpxInstObj        *instObj;
    VpsHal_VpdmaRegionDataDescParams  regDesc;
    VpsHal_VpdmaInDescParams stenDescParam;


    GT_assert(GrpxCoreTrace, (NULL != descMem));
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    if (TRUE == chObj->state.isMultiWinMode)
    {
        regParams = &chObj->mInfo.regParam[regId];
    }
    else
    {
        regParams = &chObj->regParam;
    }


    /* For first region, totally three data descriptors CLUT, Stenciling and
     * Data. For the rest regions, only two data descriptors: stenciling and Data.
     */
    for (descSet = 0u; descSet < instObj->numDescPerReg; descSet++)
    {
        /*Get the right descriptor memory based on the region num*/
        if (0u == regId)
        {
            dataDesc =(Ptr)descMem->inDataDesc[1u + descSet];
        }
        else
        {
            dataDesc =(Ptr)descMem->multiWinDataDesc[VCORE_GRPX_DESCS_PER_REGION *
                                        (regId - 1u)  + descSet];
        }
        /*stenciling descriptor comes first */
        if (descSet == 0u)
        {
            if (TRUE == regParams->stencilingEnable)
            {
                retVal = vcoreGrpxCreateStenDataDesc(hObj, chObj,
                     regParams, &stenDescParam, dataDesc,regId);
            }
            else
            {
                /*create dummy to replace the real descriptor one*/
                VpsHal_vpdmaCreateDummyDesc(dataDesc);
                VpsHal_vpdmaCreateDummyDesc(
                               (Ptr)((UInt32)dataDesc +
                               VPSHAL_VPDMA_CONFIG_DESC_SIZE));
            }
        }
        else
        {
            /*create grpx data descriptor*/
            retVal = vcoreGrpxCreateGrpxDataDesc(hObj,
                                         chObj,
                                         regParams,
                                         &regDesc,
                                         dataDesc,
                                         regId);
        }
    }
    return (retVal);
}

/**
 *  vcoreGrpxRtProgramOvlyMem
 *  runtime program the overlay memory for configuration descriptor
 *  return VPS_SOK on success else return error values
 */

static Int32 vcoreGrpxRtProgramOvlyMem(Vcore_GrpxHandleObj *hObj,
                                       Vcore_GrpxChObj *chObj,
                                       const Vcore_DescMem *descMem,
                                       UInt32 regId)
{
    Int32                    retVal = VPS_SOK;
    Ptr                      mem;
    Vcore_GrpxInstObj        *instObj;
    Vps_GrpxScParams         *scParams;
    VpsHal_GrpxVpdmaOvlyInfo ovly;

    GT_assert(GrpxCoreTrace, (NULL != descMem));
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));


    scParams = &chObj->scParams;
    VpsHal_grpxGetOvlyInfo(instObj->grpxHandle,
            VPSHAL_GRPX_VPDMA_OVLY_SC_ONLY_ATTR,
            &ovly);

    mem = (Ptr)((UInt32)descMem->coeffOvlyMem+
            ovly.offsetOfAttr * 4u);

    retVal = VpsHal_grpxSetSc(instObj->grpxHandle,
            scParams,
            mem);
    if (VPS_SOK == retVal)
    {
        GT_1trace(GrpxCoreTrace,
                GT_DEBUG,"Scalar Coeff Desc Update at 0x%0.8x.\n",(UInt32)mem);
    }
    else
    {
        GT_0trace(GrpxCoreTrace,
                GT_ERR,"Update scaling coeff failed.\n");

    }
    return (retVal);
}

