/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_grpx.c
 *
 *  \brief VPS Graphic HAL Header File
 *  This file exposes the APIs of the Graphic HAL to the upper layers.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/cslr/cslr_vps_grpx.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/hal/src/vpshalGrpxDefaults.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*none*/
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/**
 *  \brief struct VpsHal_grpxInstInfo
 *  There are three instances of Grpx in VPS. This structure is used to
 *  store instance specific information.
 */
typedef struct
{
    VpsHal_GrpxInst          grpxPort;
    /**< GRPX PORT */
    VpsHal_GrpxVpdmaOvlyInfo *ovlyInfo;
    /**< vpdma overlay info */
    VpsHal_GrpxFrmDispAttr   frmAttr;
    /**< contain the current frame information */
    UInt32                   openCnt;
    /**< open number */
} VpsHal_GrpxInstInfo;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 halGrpxInitInstances(VpsHal_GrpxInstInfo *grpxInst);

/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */
/**
 *  GRPX Instance - Module variable to store information about each GRPX
 *  instance.
 */
static  VpsHal_GrpxInstInfo GrpxInfo[VPSHAL_GRPX_INST_MAX];

/**
 *  GRPX Vpdam Overlay info - Module variable to store information about each
 *  GRPX
 *  overlay information.
 */
static  VpsHal_GrpxVpdmaOvlyInfo
                 GrpxVpdmaAttrOvly [VPSHAL_GRPX_VPDMA_OVLY_ATTR_MAX];

/**
 *  \brief Scaling Coefficiengts Array
 */

/** \brief horizontal up-scaling coefficients*/
static UInt16 GrpxScHorzUpScaleCoeff[5][8] =
                    GRPX_SC_HORIZONTAL_UP_SCALE_COEFF   ;
/** \brief verticl up-scaling coefficients*/
static UInt16 GrpxScVertUpScaleCoeff[4][8] =
                    GRPX_SC_VERTICAL_UP_SCALE_COEFF;
/** \brief horizontal down-scaling coefficients*/
static UInt16 GrpxScHorzDownScaleCoeff[4][5][8] =
                    GRPX_SC_HORIZONTAL_DOWN_SCALE_COEFF;
/** \brief vertical down-scaling coefficients*/
static UInt16 GrpxScVertDownScaleCoeff[4][4][8] =
                    GRPX_SC_VERTICAL_DOWN_SCALE_COEFF;
/** \brief horizontal anti-flicker coefficients */
static UInt16 GrpxScHorzAFCoeff[5][8] =
                    GRPX_SC_HORIZONTAL_AF_COEFF;
/** \brief vertical anti-flicker coefficients */
static UInt16 GrpxScVertAFCoeff[4][8] =
                    GRPX_SC_VERTICAL_AF_COEFF;
/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */

/**
 *  instGrpxCheck
 *   Function to check whether the handle is the right value
 *
 */
static Int32 inline instGrpxCheck(VpsHal_GrpxHandle handle)
{
    Int                      ret = VPS_EFAIL;
    UInt32                   instCnt;
    VpsHal_GrpxInstInfo      *temInst;

    GT_assert(VpsHalTrace,(NULL != handle));
    temInst = (VpsHal_GrpxInstInfo *)handle;

    for (instCnt = 0u; instCnt < VPSHAL_GRPX_INST_MAX; instCnt++)
    {
        if (temInst == &GrpxInfo[instCnt])
        {
            ret = VPS_SOK;
            break;
        }
    }

    return (ret);

}
/**
 *  VpsHal_grpxInit
 *  \brief  GRPX HAL Init Function
 *
 *  This function should be initialized prior to calling any graphic function
 *  Function to initialize graphic driver. It initializes global structure
 *  scInfo, sets the peaking and edge detect parameters in the registers and
 *  gets the VPDMA register overlay size and virtual register offsets in the
 *  overlay from the VPDMA. Since VPDMA register overlay size and virtual
 *  register offsets are non-changing and same for all scalars, these are set
 *  at initial time.
 *
 *  \param numInstances [IN] Number of instance objects to be initialized
 *  \param initParams   [IN] Pointer to the instance parameter containing
 *                      instance specific information. If the number of instance
 *                      is greater than 1, then this pointer should point to an
 *                      array of init parameter structure of size numInstances.
 *                      This parameter should not be NULL.
 *  \param arg          [IN] Not currently used. Meant for future purpose.
 *
 *  \return       Returns VPS_SOK on success else returns error value
 *
 */
Int VpsHal_grpxInit(UInt32 numInstances,
                    const VpsHal_GrpxInstParams *initParams,
                    Ptr arg)
{
    Int                      portCnt;
    Int                      retVal = VPS_SOK;

    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_GRPX_INST_MAX));
    GT_assert(VpsHalTrace, (initParams != NULL));

    retVal = halGrpxInitInstances(NULL);
    VpsUtils_memset(GrpxInfo,0u,sizeof(GrpxInfo));
    for (portCnt = 0u; portCnt < numInstances; portCnt++)
    {
        GT_assert(VpsHalTrace,
            (initParams[portCnt].portId < VPSHAL_GRPX_INST_MAX));
        GrpxInfo[portCnt].grpxPort = initParams[portCnt].portId;
        GrpxInfo[portCnt].ovlyInfo =  &GrpxVpdmaAttrOvly[0];
    }

   return (retVal);
}

/**
 * halGrpxInitInstances
 * used to init the graphic instance by set the VPDMA configuration information
**/
static Int halGrpxInitInstances(VpsHal_GrpxInstInfo *grpxInst)
{


    /*frame display attribute*/
    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_FRAME_ONLY_ATTR].destAddr =
                                       GRPX_FRAME_DISPLAY_ATTR_ADDR_OFFSET;

    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_FRAME_ONLY_ATTR].sizeOfOvly =
                                           GRPX_VDPMA_OVLY_FRAME_ATTR_SIZE;

    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_FRAME_ONLY_ATTR].offsetOfAttr =
                                       GRPX_REGION_FRAME_DISPLAY_ATTR_OFFSET;
    /*whole GRPX attributes   */
    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_ALL_ATTRS].destAddr =
                                       GRPX_FRAME_DISPLAY_ATTR_ADDR_OFFSET;

    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_ALL_ATTRS].sizeOfOvly =
                                  GRPX_VPDMA_OVLY_ALL_ATTRS_SIZE;

    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_ALL_ATTRS].offsetOfAttr =
                                  GRPX_REGION_FRAME_DISPLAY_ATTR_OFFSET;
     /*whole scaler coefficients and configs   */
    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_SC_ONLY_ATTR].destAddr =
                                       GRPX_REGION_SC_ATTR1_ADDR_OFFSET;

    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_SC_ONLY_ATTR].sizeOfOvly =
                                 GRPX_VPDMA_OVLY_ALL_COEFFS_CONFIGS_SIZE;

    GrpxVpdmaAttrOvly[VPSHAL_GRPX_VPDMA_OVLY_SC_ONLY_ATTR].offsetOfAttr =
                                GRPX_REGION_SC_ATTR1_OFFSET;

    return (VPS_SOK);

}

/**
 *  VpsHal_grpxDeInit
 *  \brief  GRPX HAL Exist Function
 *
 *  \param arg   [IN] Not Currently used. Meant for future purpose.
 *
 *  \return      Returns VPS_SOK on success else returns error value
 */
Int VpsHal_grpxDeInit(Ptr arg)
{
    UInt32                   instCnt;

    /*reset the count number*/
    for (instCnt = 0u; instCnt < VPSHAL_GRPX_INST_MAX; instCnt++)
    {
     /*clear whole structure back to default*/
        GrpxInfo[instCnt].openCnt = 0u;
        GrpxInfo[instCnt].grpxPort = VPSHAL_GRPX_INST_MAX;
        GrpxInfo[instCnt].frmAttr.frameHeight = 0u;
        GrpxInfo[instCnt].frmAttr.frameWidth = 0u;
        GrpxInfo[instCnt].frmAttr.interlacedSrc = 0u;
    }
    return (VPS_SOK);
}



/**
 *  VpsHal_GrpxHandle VpsHal_grpxOpen
 *  \brief  GRPX HAL Open Function
 *
 *  This Function is used to return  graphics handle . This function must be
 *  called before using any graphics. Handle returned from this function must
 *  be used in all other functions to set/get parameters correctly.
 *
 *  \param grpxPort [IN] request graphics port
 *
 *  \return handle     Returns GRPX instance handle on success else
 *                         returns NULL
 */
VpsHal_GrpxHandle VpsHal_grpxOpen(VpsHal_GrpxInst grpxPort)
{
    UInt32                   instCnt, cookie;
    VpsHal_GrpxHandle        handle = NULL;

    GT_assert(VpsHalTrace,(grpxPort < VPSHAL_GRPX_INST_MAX));

    for (instCnt = 0u; instCnt < VPSHAL_GRPX_INST_MAX; instCnt++)
    {
        if (grpxPort == GrpxInfo[instCnt].grpxPort)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0 == GrpxInfo[instCnt].openCnt)
            {
                handle =(VpsHal_GrpxHandle) &(GrpxInfo[instCnt]);
                GrpxInfo[instCnt].openCnt++;
            }

            /* Enable global Interrupts */
            Hwi_restore(cookie);

            break;
        }
    }


    return (handle);
}

/**
 *  VpsHal_grpxClose
 *  \brief  GRPX HAL Close Function
 *
 *  Function to close/release the graphics handle.
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle  [IN]graphic handle
 *
 *  \return  Return VPS_SOK on success else returns error value
 */
Int VpsHal_grpxClose(VpsHal_GrpxHandle handle)
{
    Int                      ret = VPS_EFAIL;
    UInt32                   cookie;
    VpsHal_GrpxInstInfo      *grpxInfo;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace,(0 == instGrpxCheck(handle)));

    grpxInfo = (VpsHal_GrpxInstInfo *)handle;

    /* Disable Hardware Interrupts */
    cookie = Hwi_disable();
    if (grpxInfo->openCnt > 0u)
    {
        grpxInfo->openCnt--;
        /*init the frame attribute*/
        if (grpxInfo->openCnt == 0u)
        {
            grpxInfo->frmAttr.frameHeight = 0u;
            grpxInfo->frmAttr.frameWidth = 0u;
            grpxInfo->frmAttr.interlacedSrc = 0u;
        }
        ret = VPS_SOK;
    }
    /* Enable Hardware Interrupts */
    Hwi_restore(cookie);

    return (ret);
}



/**
 *  VpsHal_grpxSetFrameAttr
 *  \brief  GRPX HAL Set Frame Attribute Function
 *
 *    Function to set the frame display attribute in the overlay memory to
 *    create configuration descriptor used by the VPDMA.  This function should
 *    be called before any region attribute VPDMA transaction. The reset
 *    parameter is used to reset the corresponding graphics, even so, dispAttr
 *    should be still filled with the normal value to let the hardware work
 *    properly after reset. In order to get the offset and size(word) of this
 *    configuration in the overlay memory, call Grpx_getOvlyInfo.  The return
 *    information  is used for the VPDMA.
 *
 *  \param handle         [IN] Graphic port handle
 *  \param dispAttr       [IN] predefined memory location to store the frame
 *                                  display information
 *  \param reset          [IN] reset or not
 *  \param configOvlyPtr  [IN] pointer to the configuration overlay memory,
 *                            it can not be null for GRPX module
 *
 *  \return    returns VPS_SOK on success or return error values
 */
Int VpsHal_grpxSetFrameAttr(VpsHal_GrpxHandle handle,
                            VpsHal_GrpxFrmDispAttr *dispAttr,
                            UInt32 reset, Ptr configOvlyPtr)
{
    Int                      ret = VPS_SOK;
    UInt32                   *virRegOffset;
    VpsHal_GrpxInstInfo      *instInfo;

    GT_assert(VpsHalTrace,(0 == instGrpxCheck(handle)));
    GT_assert(VpsHalTrace,(NULL != dispAttr));
    GT_assert(VpsHalTrace,(NULL != configOvlyPtr));

    /* the input width and heigth has limitation*/
    GT_assert(VpsHalTrace,
             (CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_HEIGHT_MAX
                                            > dispAttr->frameHeight));
    GT_assert(VpsHalTrace,
             (CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_WIDTH_MAX
                                      > dispAttr->frameWidth));

    instInfo = (VpsHal_GrpxInstInfo *)handle;
    /*get the attribute offset  */
    virRegOffset =  (UInt32 *)configOvlyPtr;

    /* write to the config overlay area*/
    /* frame height should be even number, for interlaced input,
     * it is field height
     */
    if (dispAttr->interlacedSrc)
        *(virRegOffset + 1u) = (((UInt32)((dispAttr->frameHeight & (~1)) >> 1)
          << CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_HEIGHT_SHIFT) &
                  CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_HEIGHT_MASK);
    else
        *(virRegOffset + 1u) = (((UInt32)(dispAttr->frameHeight & (~1))  <<
                  CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_HEIGHT_SHIFT) &
                  CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_HEIGHT_MASK);

    /*write the frame_width to the config overlay area*/
    *(virRegOffset + 1u) |= (((UInt32) dispAttr->frameWidth  <<
                   CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_WIDTH_SHIFT) &
                    CSL_VPS_GRPX_FRAME_DISPLAY_ATTRIBUTE_FRAME_WIDTH_MASK);

    /*set the interlaced flag*/
    if (dispAttr->interlacedSrc)
    {
        *(virRegOffset + 3u) =
             CSL_VPS_GRPX_FRAME_DISPLAY_SRC_FMT_INTERLACED_MASK;
    }
    else
    {
        *(virRegOffset + 3u) &=
            ~CSL_VPS_GRPX_FRAME_DISPLAY_SRC_FMT_INTERLACED_MASK;
    }

    /*set the reset flag*/
    if (reset)
    {
        *(virRegOffset + 3u) |=CSL_VPS_GRPX_FRAME_DISPLAY_SOFT_RESET_MASK;
    }

   /*store the frame information to the handle for the future usage*/
    instInfo->frmAttr.frameWidth = dispAttr->frameWidth;
    instInfo->frmAttr.frameHeight = dispAttr->frameHeight & (~1);
    instInfo->frmAttr.interlacedSrc = dispAttr->interlacedSrc;
    return (ret);
}

/**
 *  VpsHal_grpxGetStatus
 *  \brief  GRPX HAL Get Status Function
 *
 *    Function to return the current GRPX interrupt error status and
 *         related error information
 *
 *  \param handle    [IN] GRPX port handle
 *  \param intStatus [OUT] pointer to the structure contained all information
 *
 *  \return  Return VPS_SOK on success or return error values
 */
Int VpsHal_grpxGetStatus(VpsHal_GrpxHandle handle,
                         VpsHal_GrpxIntStatus  *intStatus)
{
    Int                      ret = VPS_SOK;
    UInt8                    client;
    UInt32                   grpxData[4];
    VpsHal_GrpxInstInfo      *instInfo = NULL;

    /*input parameter checking*/

    GT_assert(VpsHalTrace, (0 == instGrpxCheck(handle)));
    GT_assert(VpsHalTrace, (NULL !=intStatus));

    instInfo = (VpsHal_GrpxInstInfo *)handle;

    /*call the vpdma read function to return the data*/
    client = (UInt8)VPSHAL_VPDMA_CLIENT_GRPX0 + (UInt8)instInfo->grpxPort;
    /* client number, address, length, return pointer */
    ret = VpsHal_vpdmaReadConfig((VpsHal_VpdmaClient)client,4,4,grpxData);

    /*parse the data and return to the caller*/
    if (VPS_SOK == ret)
    {
        intStatus->vpiDataFlushStatus =
                (UInt32)((grpxData[0] &
                   CSL_VPS_GRPX_FRAME_DISPLAY_VPI_DATA_FLUSH_STATUS_MASK) >>
                     CSL_VPS_GRPX_FRAME_DISPLAY_VPI_DATA_FLUSH_STATUS_SHIFT);

        intStatus->frameCfgErrorInt =
                (UInt32)((grpxData[0] &
                   CSL_VPS_GRPX_FRAME_DISPLAY_FRAME_CFG_ERROR_INT_MASK) >>
                      CSL_VPS_GRPX_FRAME_DISPLAY_FRAME_CFG_ERROR_INT_SHIFT);

        intStatus->eolMissingInt =
               (UInt32)((grpxData[0] &
                  CSL_VPS_GRPX_FRAME_DISPLAY_EOL_MISSING_INT_MASK) >>
                      CSL_VPS_GRPX_FRAME_DISPLAY_EOL_MISSING_INT_SHIFT);

        intStatus->eofMissingInt =
              (UInt32)((grpxData[0] &
                 CSL_VPS_GRPX_FRAME_DISPLAY_EOF_MISSING_INT_MASK) >>
                   CSL_VPS_GRPX_FRAME_DISPLAY_EOF_MISSING_INT_SHIFT);

        intStatus->faultyPosInt =
            (UInt32)((grpxData[0] &
               CSL_VPS_GRPX_FRAME_DISPLAY_FAULTY_POS_INT_MASK) >>
                  CSL_VPS_GRPX_FRMAE_DISPLAY_FAULTY_POS_INT_SHIFT);

        intStatus->faultyRegId =
                (UInt8)((grpxData[0] &
                 CSL_VPS_GRPX_FRAME_DISPLAY_FAULTY_REGION_INT_MASK) >>
                     CSL_VPS_GRPX_FRAME_DISPLAY_FAULTY_REGION_INT_SHIFT);
        intStatus->vPosition =
                (UInt16)((grpxData[2] &
                  CSL_VPS_GRPX_FRAME_DISPLAY_VERTICAL_POSITION_MASK) >>
                      CSL_VPS_GRPX_FRAME_DISPLAY_VERTICAL_POSITION_SHIFT);

        intStatus->hPosition =
                (UInt16)((grpxData[2] &
                   CSL_VPS_GRPX_FRAME_DISPLAY_HORIZONTAL_POSITION_MASK) >>
                       CSL_VPS_GRPX_FRAME_DISPLAY_HORIZONTAL_POSITION_SHIFT);
        }


    return (ret);

}


/**
 *  VpsHal_grpxSetSc
 *  \brief  GRPX HAL Set scaler Function
 *
 *  Function to set the scaler coefficient and factors
 *
 *  \param handle        [IN] graphics port handle
 *  \param scParams      [IN] scaler factors
 *  \param configOvlyPtr [IN] predefined memory to store the coefficients
 *
 *  \return  Return VPS_SOK on success or return error values
 */
Int VpsHal_grpxSetSc(VpsHal_GrpxHandle handle,
                     Vps_GrpxScParams *scParams,
                     Ptr configOvlyPtr)

{
    Int                      ret = VPS_SOK;
    UInt32                   *virRegOffset;
    Vps_GrpxScCoeff          *coeff;
    UInt16                   *hor_coeff;
    UInt16                   *ver_coeff;
    Grpx_ScScaleSet          horScaleSet, verScaleSet;
    UInt32                   hor_scale_cnt,ver_scale_cnt;

    GT_assert(VpsHalTrace, (0 == instGrpxCheck(handle)));
    GT_assert(VpsHalTrace, (NULL != (Void *)scParams));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /*checking parameters*/
    if ((scParams->inHeight == 0u) || (0u == scParams->inWidth)
        || (0u == scParams->outHeight) || (0u == scParams->outHeight))
    {
        return (VPS_EINVALID_PARAMS);
    }


    /*get the horizontal default coefficient*/
    horScaleSet = getScaleSet(handle, scParams->inWidth,
                        scParams->outWidth, &hor_scale_cnt);
    /* get the vertical default coefficient*/
    verScaleSet = getScaleSet(handle,scParams->inHeight,
            scParams->outHeight, &ver_scale_cnt);

    /* prepare the coefficient to use: default or user defined
     for the AF, horizontal use upscaling coeff, while vertical
     use 0.5 coeff*/
    if (NULL == scParams->scCoeff)
    {
        if (GRPX_SC_US_SET == horScaleSet)
            hor_coeff = &GrpxScHorzUpScaleCoeff[0][0];
        else if (GRPX_SC_AF == horScaleSet)
            hor_coeff = &GrpxScHorzAFCoeff[0][0];
        else
            hor_coeff = &GrpxScHorzDownScaleCoeff[horScaleSet][0][0];

        if (GRPX_SC_US_SET == verScaleSet)
            ver_coeff = &GrpxScVertUpScaleCoeff[0][0];
        else if (GRPX_SC_AF == verScaleSet)
            ver_coeff = &GrpxScVertAFCoeff[0][0];
        else
            ver_coeff = &GrpxScVertDownScaleCoeff[verScaleSet][0][0];
    }
    else
    {
        /* use the coefficient from user directly*/
        coeff= scParams->scCoeff;
        hor_coeff = coeff->hsCoeff;
        ver_coeff = coeff->vsCoeff;
    }
    /*the scaled region should not over the frame*/
    virRegOffset = (UInt32 *)configOvlyPtr;

    /*load the horizontal coefficients*/
    userCoeffCopy(handle,(Ptr)virRegOffset,hor_coeff,
                                   VPSHAL_GRPX_HOR_SC_COEFF_SIZE);

    /*move the point to the attribute 6*/
    virRegOffset += GRPX_VPDMA_OVLY_HOR_COEFFS_SIZE;

    *virRegOffset =(hor_scale_cnt <<
              CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_COUNTVALHORZ_SHIFT ) &
              CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_COUNTVALHORZ_MASK;

    /*set the start offset*/
    *virRegOffset |= ((UInt32)scParams->horFineOffset <<
               CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_FINEOFFSETHORZ_SHIFT) &
               CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_FINEOFFSETHORZ_MASK;

    /*move pointed to the attribute 7*/
    virRegOffset += GRPX_VPDMA_OVLY_SC_CONFIG_SIZE;
    /*load the horizontal coefficients*/
    userCoeffCopy(handle,(Ptr)virRegOffset,ver_coeff,
            VPSHAL_GRPX_VER_SC_COEFF_SIZE);

    /*move pointer to attribute 11*/
    virRegOffset += GRPX_VPDMA_OVLY_VER_COEFFS_SIZE;

    *virRegOffset = (ver_scale_cnt <<
                  CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_COUNTVALHORZ_SHIFT) &
                   CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_COUNTVALHORZ_MASK;

    *virRegOffset |= ((UInt32)scParams->verFineOffset <<
                 CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_FINEOFFSETHORZ_SHIFT) &
                 CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE6_FINEOFFSETHORZ_MASK;

   return (ret);
}

/**
 *  VpsHal_grpxGetRegionYEnd
 *  \brief  GRPX HAL Get current Region Vertical End offset Function
 *
 *  This function is to calcuate the vertical end position of the current
 *   region
 *
 *  \param regDispAttr [IN] pointer to the structure containing the region
 *                          display attribute information
 *  \param scParams    [IN] scaling ration
 *
 *  \return   Return VPS_SOK on success else returns error value
 *
 */
Int VpsHal_grpxGetRegionYEnd(Vps_GrpxRegionParams *regDispAttr,
                             Vps_GrpxScParams *scParams)
{

    int yOffset;

    GT_assert(VpsHalTrace, (NULL != regDispAttr));
    GT_assert(VpsHalTrace,
             (!((NULL == scParams) && (TRUE == regDispAttr->scEnable))));


    if (regDispAttr->scEnable)
    {
        yOffset = ((UInt32)regDispAttr->regionHeight *
                          (UInt32)scParams->outHeight +
                                   ((UInt32)scParams->inHeight >> 1)) /
                                        scParams->inHeight;

        yOffset += regDispAttr->regionPosY +
                GRPX_SCALED_REGION_EXTRA_LINES;

    }
    else
    {
        yOffset = regDispAttr->regionHeight + regDispAttr->regionPosY;
    }

    return yOffset;
}

/**
 *  VpsHal_grpxRegionParamsCheck
 *  \brief  GRPX HAL Region Check Function
 *
 *  This function is to check the region display attribute to make
 *  sure this is correct
 *
 *  \param handle      [IN] Not currently used. Meant for future purpose.
 *  \param frmAttr     [IN] frame attribute
 *  \param regDispAttr [IN] pointer to the structure containing the region
 *                          display attribute information
 *  \param scParams    [IN] scaling ration
 *  \param start       [IN] first region or not
 *
 *  \return   Return VPS_SOK on success else returns error value
 *
 */
Int VpsHal_grpxRegionParamsCheck(VpsHal_GrpxHandle handle,
                                 VpsHal_GrpxFrmDispAttr  *frmAttr,
                                 Vps_GrpxRegionParams *regDispAttr,
                                 Vps_GrpxScParams *scParams,
                                 UInt32 prevY)
{
    Int                      ret = VPS_SOK;
    UInt32                   endOfRegHorz;
    UInt32                   endOfRegVert;
    UInt32                   scaledLines = 0u;
    UInt32                   scHOut;
    UInt32                   scVOut;
    Grpx_RegionScaledType    verScaledType;
    Grpx_RegionScaledType    horScaledType;

    GT_assert(VpsHalTrace, (0 == instGrpxCheck(handle)));
    GT_assert(VpsHalTrace, (NULL != frmAttr));
    GT_assert(VpsHalTrace, (NULL != regDispAttr));
    GT_assert(VpsHalTrace,
             (!((NULL == scParams) && (TRUE == regDispAttr->scEnable))));

    if (regDispAttr->scEnable)
    {
        if ((scParams->inHeight == 0u) || (0u == scParams->inWidth)
            || (0u == scParams->outHeight) || (0u == scParams->outHeight))
        {
            return (VPS_EINVALID_PARAMS);
        }
    }

    /**
     * due to the hardware scaler limitation, the scaler will output more lines
     * and pixes than what asked, this should be taken into
     * consideration when doing the frame boundary check and regions overlap
     */
    if (regDispAttr->scEnable)
    {
        /*get the current region scaled type*/
        if (scParams->inHeight > scParams->outHeight)
        {
            verScaledType = GRPX_REGION_DOWN_SCALED;
        }
        else if (scParams->inHeight < scParams->outHeight)
        {
            verScaledType = GRPX_REGION_UP_SCALED;
        }
        else
        {
            /*This is for anti-flicker only, which is use the scaler to do*/
            verScaledType = GRPX_REGION_NO_SCALED;
        }

        if (scParams->inWidth == scParams->outWidth)
        {
            horScaledType = GRPX_REGION_NO_SCALED;
        }
        else
        {
            horScaledType = GRPX_REGION_UP_SCALED;
        }

    }
    else
    {
        horScaledType = GRPX_REGION_NO_SCALED;
        verScaledType = GRPX_REGION_NO_SCALED;
    }

    /*output at most 2 line and 2 pixes when scaled*/
    if (GRPX_REGION_NO_SCALED != verScaledType)
    {
        if ((TRUE == regDispAttr->firstRegion) &&
              (TRUE == regDispAttr->lastRegion))
        {
            scVOut = scParams->outHeight;
        }
        else
        {
            scVOut = ((UInt32)regDispAttr->regionHeight *
                              (UInt32)scParams->outHeight +
                                       ((UInt32)scParams->inHeight >> 1)) /
                                            scParams->inHeight;
        }

        endOfRegVert = regDispAttr->regionPosY + scVOut +
                GRPX_SCALED_REGION_EXTRA_LINES;

    }
    else
    {
        endOfRegVert = regDispAttr->regionPosY + regDispAttr->regionHeight;
    }

    if (GRPX_REGION_NO_SCALED != horScaledType)
    {
        if ((TRUE == regDispAttr->firstRegion) &&
              (TRUE == regDispAttr->lastRegion))
        {
            scHOut = scParams->outWidth;
        }
        else
        {
            scHOut = ((UInt32)regDispAttr->regionWidth *
                            (UInt32)scParams->outWidth +
                                   ((UInt32)scParams->inWidth >> 1)) /
                                        scParams->inWidth;
        }
        endOfRegHorz = regDispAttr->regionPosX + scHOut +
                GRPX_SCALED_REGION_EXTRA_PIXES;

    }
    else
    {
        endOfRegHorz = regDispAttr->regionPosX + regDispAttr->regionWidth;

    }

    /*make sure the size is not out of the frame*/
    if ((endOfRegHorz > frmAttr->frameWidth) ||
       (endOfRegVert > (frmAttr->frameHeight >> frmAttr->interlacedSrc)))
    {
        return (VPS_EFAIL);
    }

    /*Not allow vertical overlap*/
    if (FALSE ==regDispAttr->firstRegion)
    {
        /*we need make sure the region are not overlap on vertical*/
        if ( GRPX_REGION_NO_SCALED == verScaledType)
        {
            scaledLines = 0u;
        }
        else if (GRPX_REGION_UP_SCALED == verScaledType)
        {
           /* for the up scaled, we need one line ahead for line buffer*/
           scaledLines = GRPX_REGION_UP_SCALED_GAP;
        }
        else
        {
           scaledLines = GRPX_REGION_DOWN_SCALED_GAP;
           /* for the down scaled, we need two line ahead for line buffer*/
        }
        /*perform overlap checking*/
        if (regDispAttr->regionPosY < (prevY + scaledLines))
        {
            return (VPS_EFAIL);
        }
    }

   return (ret);
}

/**
 *  VpsHal_grpxRegionDispAttrSet
 *  \brief  GRPX HAL Set Region Attribute Function
 *
 *   Function is to set the region display attribute to the VPDMA descriptor
 *
 *  \param handle       [IN] Graphics port handle.
 *  \param regDispAttr  [IN] pointer to the structure containing the region
 *                           display attribute information
 *  \param descInfo     [OUT] pointer to the Vpdma region attribute structure
 *
 *  \return    Return VPS_SOK on success else returns error value
 */
Int VpsHal_grpxRegionDispAttrSet (VpsHal_GrpxHandle handle,
                                  Vps_GrpxRegionParams*regDispAttr,
                                  VpsHal_VpdmaRegionDataDescParams *descInfo)
{
    Int                      ret = VPS_SOK;

    /*set up the region width and height*/
    descInfo->regionWidth = (UInt16)regDispAttr->regionWidth;
    descInfo->regionHeight = (UInt16)regDispAttr->regionHeight;

    /*set up the region start location*/
    descInfo->startX = (UInt16)regDispAttr->regionPosX;
    descInfo->startY = (UInt16)regDispAttr->regionPosY;

    /*set up the region priority*/
    descInfo->regionPriority = (UInt8)regDispAttr->dispPriority;

    /*
      setup the region attribute based on the input
      In the region data descriptor, the first/last,scaler/af/stencil and
      bbox enable field are stored
      in the field regionAttr as the following:
      ____________________________________________________________________
      |b16  | b15-b9   | b8   |  b7|b6|b5|b4          |b3|b2      |b1  |b0 |
      |trans|reserved  |bbox  |  reserved |stencil    |rs |scaler |last|first|
      | ___________________________________________________________________|
      the transparency was stored in the bit 16 of regionAttr
    */
    if (regDispAttr->firstRegion)
    {
        descInfo->regionAttr =
            CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_DISP_FIRST_REG_MASK ;
    }
    else
    {
        descInfo->regionAttr &=
                 ~CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_DISP_FIRST_REG_MASK;
    }

    if (regDispAttr->lastRegion)
    {
        descInfo->regionAttr |=
           CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_DISP_LAST_REG_MASK;
    }
    else
    {
        descInfo->regionAttr &=
                   ~CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_DISP_LAST_REG_MASK;
    }
    /*enable the scaler*/
    if (regDispAttr->scEnable)
    {
        descInfo->regionAttr |=
                     CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_SCALER_ENABLE_MASK;
    }
    else
    {
        descInfo->regionAttr &=
                   ~CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_SCALER_ENABLE_MASK;
    }

    /*enable the stenciling*/
    if (regDispAttr->stencilingEnable)
    {
         descInfo->regionAttr |=
                     CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_STENCILING_EN_MASK;
    }
    else
    {
     descInfo->regionAttr &=
                    ~CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_STENCILING_EN_MASK;
    }

    /*enable the boundBox*/
    if (regDispAttr->bbEnable)
    {
        descInfo->regionAttr |=
                     CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_BOUND_BOX_EN_MASK;
    }
    else
    {
        descInfo->regionAttr &=
                    ~CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_BOUND_BOX_EN_MASK;
    }

    descInfo->regionAttr >>=
          CSL_VPS_GRPX_REGION_DISPLAY_ATTRIBUTE_DISP_FIRST_REG_SHIFT;

    /*set the bound-box and blending alpha value*/
    descInfo->bbAlpha= (UInt8)regDispAttr->bbAlpha;
    descInfo->blendAlpha = (UInt8)regDispAttr->blendAlpha;

    /*set the blend type*/
    descInfo->blendType = (VpsHal_VpdmaGrpxBlendType)regDispAttr->blendType;

    /*set up the transparency information*/
    if (regDispAttr->transEnable)
    {
        descInfo->regionAttr |= VPSHAL_VPDMA_GRPX_TRANSPARENCY;
    }
    else
    {
        descInfo->regionAttr &= ~VPSHAL_VPDMA_GRPX_TRANSPARENCY;
    }

   descInfo->transMask = (VpsHal_VpdmaGrpxTransMask)regDispAttr->transType;
   descInfo->transColor = regDispAttr->transColorRgb24;

    return (ret);
}


/**
 *  VpsHal_grpxGetOvlyInfo
 *  \brief  GRPX HAL Get Overlay Function
 *
 *     Function to get the attribute hardware offset(128Bit for VPDMA
 *     transfer), size (word size for VPDMA transfer) and attribute word
 *     offset(to get the payload start address) in the current configuration
 *     descriptor overlay. These two information are used to fill the
 *     requirement in the VPMDA create configuration descriptor function.
 *     Moreover, the destAdd(attribute offset) is also need to
 *
 *  \param handle     [IN] grpx port
 *  \param ovlyType   [IN] point to the attribute type
 *  \param ovlyInfo   [OUT] overlay information for the assigned type
 *
 *  \return   Return 0 on success else returns error value
 */
Int VpsHal_grpxGetOvlyInfo(VpsHal_GrpxHandle handle,
                           VpsHal_GrpxVpdmaAttrOvly ovlyType,
                           VpsHal_GrpxVpdmaOvlyInfo *ovlyInfo)
{
    Int                      ret = VPS_SOK;
    VpsHal_GrpxVpdmaOvlyInfo *tempOvlyInfo;

    GT_assert(VpsHalTrace, (NULL != ovlyInfo));
    GT_assert(VpsHalTrace, (0 == instGrpxCheck(handle)));

    tempOvlyInfo = &GrpxVpdmaAttrOvly[ovlyType];
    /*return the information which is useful for the VPDMA*/
    ovlyInfo->destAddr = tempOvlyInfo->destAddr;
    ovlyInfo->sizeOfOvly = tempOvlyInfo->sizeOfOvly;
    ovlyInfo->offsetOfAttr = tempOvlyInfo->offsetOfAttr;
    return (ret);
}

/**
 *  VpsHaL_GrpxgetGrpxFrmAttrs
 *  \brief  GRPX HAL Get Frame Attribute Function
 *
 *  This function is to read the current frame display attributes of the
 *       assigned Graphic ported
 *
 *  \param handle   [IN] specify the GRPX port
 *  \param dispAttr [OUT] pointer to store graphic frame display attributes
 *
 *  \return Return 0 on success else returns error value
*/
Int VpsHal_grpxGetFrmAttrs(VpsHal_GrpxHandle handle,
                           VpsHal_GrpxFrmDispAttr *dispAttr)
{
    Int                      ret = VPS_SOK;
    VpsHal_GrpxInstInfo      *instInfo;

    GT_assert(VpsHalTrace, (0 == instGrpxCheck(handle)));
    GT_assert(VpsHalTrace, (NULL != dispAttr));

    instInfo = (VpsHal_GrpxInstInfo *)handle;
    dispAttr->frameHeight = instInfo->frmAttr.frameHeight;
    dispAttr->frameWidth =  instInfo->frmAttr.frameWidth;
    dispAttr->interlacedSrc = instInfo->frmAttr.interlacedSrc;

    return (ret);
}
