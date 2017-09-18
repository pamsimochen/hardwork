/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_grpx.h
 *
 *  \brief VPS Graphic HAL Header File
 *  This file exposes the APIs of the Graphic HAL to the upper layers.
 *
 */

#ifndef _VPSHAL_GRPX_H
#define _VPSHAL_GRPX_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief typedef for the graphics handle
 */
typedef Void * VpsHal_GrpxHandle;
/**
 *  \brief  graphcis coefficient phase
 *     total 8 phases for both horizontal and vertial coefficients
 */
#define VPSHAL_GRPX_SC_COEFF_PHASE  (0x8u)

/**
 *  \brief  graphcis horizontal coefficient tap
 *       5 taps for horizontal coefficients
 */
#define VPSHAL_GRPX_HOR_SC_COEFF_TAP (0x5u)

/**
 *  \brief  graphcis vertical coefficient tap
 *      4 taps for horizontal coefficients
 */
#define VPSHAL_GRPX_VER_SC_COEFF_TAP (0x4u)
/**
 *  \brief horizontal coefficients size
 */
#define VPSHAL_GRPX_HOR_SC_COEFF_SIZE   (VPSHAL_GRPX_SC_COEFF_PHASE * \
                                           VPSHAL_GRPX_HOR_SC_COEFF_TAP )
/**
 *  \brief vertical coefficients size(16bit)
 */

#define VPSHAL_GRPX_VER_SC_COEFF_SIZE   (VPSHAL_GRPX_SC_COEFF_PHASE * \
                                           VPSHAL_GRPX_VER_SC_COEFF_TAP )


/**
 *  \brief enum Grpx_Inst
 *  This enum is used to defines various instances of the graphics in the VPS.
 *  There are three graphics ports available in various pipelines.  This enum
 *  is used to get the handle of the scalar. */
typedef enum
{
    VPSHAL_GRPX_INST_0 = 0,
    /**< Graphic 0 */
    VPSHAL_GRPX_INST_1,
    /**< Graphic 1 */
    VPSHAL_GRPX_INST_2,
    /**< Graphic 0 */
    VPSHAL_GRPX_INST_MAX
    /**< MAX graphic port */
}VpsHal_GrpxInst;

/**
 *  \ brief Grpx_RegionOrder
 *  This enum is used to defines the display order of the region
 *  to be displayed
 */
typedef enum
{
    VPSHAL_GRPX_REGION_FIRST = 0,
    /**< The first region */
    VPSHAL_GPRX_REGION_LAST,
    /**< The last region */
    VPSHAL_GPRX_REGION_MIDDLE
    /**< neither first nor last */
} vpsHal_GrpxRegionOrder;

/**
 *  \brief Grpx_VpdmaOvlyType
 *  This enum is used to define all overlay types GRPX may use in the operation
 */
typedef enum
{
    VPSHAL_GRPX_VPDMA_OVLY_FRAME_ONLY_ATTR = 0,
    /**< frame display attribute only */
    VPSHAL_GRPX_VPDMA_OVLY_ALL_ATTRS ,
    /**<  all GRPX attriubes */
    VPSHAL_GRPX_VPDMA_OVLY_SC_ONLY_ATTR,
    /**< all grpx attribute except frame display */
    VPSHAL_GRPX_VPDMA_OVLY_ATTR_MAX
}VpsHal_GrpxVpdmaAttrOvly;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure containing instance specific parameters used at the init.
 */
typedef struct
{
    VpsHal_GrpxInst           portId;
    /**< GRPX Instance */

}VpsHal_GrpxInstParams;

/**
 *  \brief VpsHal_grpxIntStatus
 *
 *  This struct is used to define the GRPX error interrupt status with the
 *  related information to explain the error interrupt
*/
typedef struct
{
    UInt32 vpiDataFlushStatus;
    /**< VPI flush data or not */
    UInt32 frameCfgErrorInt;
    /**< frame config error*/
    UInt32 eolMissingInt;
    /**< missing end of line */
    UInt32 eofMissingInt;
    /**< missing end of frame */
    UInt32 faultyPosInt;
    /**< region position out of frame*/
    UInt8 faultyRegId;
    /**< error region id*/
    UInt16 vPosition;
    /**< vertical position*/
    UInt16 hPosition;
    /**< horizontal position */
}VpsHal_GrpxIntStatus;


/**
 *  \brief Grpx_FrmDispAttr
 *  This structure is use to config the frame display attribute
 *  which should be set prior to any region display attribute
 */
typedef struct
{
    UInt32 frameHeight;
    /**< height of incoming frame */
    UInt32 frameWidth;
    /**< width of incoming frame */
    UInt32  interlacedSrc;
    /**< interlaced(1) or progressive(0) input */
}VpsHal_GrpxFrmDispAttr;

/**
 *  \brief VpsHal_grpxVpdmaOvlyInfo
 *  This structure is used to define the descriptor overlay information
 *  which is required by the VPDMA module
 */
typedef struct
{
    UInt8  destAddr;
    /**< hardware design address */
    UInt8  sizeOfOvly ;
    /**< overlay memory size */
    UInt8  offsetOfAttr;
    /**< attribute offset in the overlya memory */
}VpsHal_GrpxVpdmaOvlyInfo;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

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
 *  \param numInstances Number of instance objects to be initialized
 *  \param initParams    [IN] Pointer to the instance parameter containing
 *                            instance specific information. If the number of
 *                            instance is greater than 1, then this pointer
 *                            should point to an array of init parameter
 *                            structure of size numInstances.  This parameter
 *                            should not be NULL.
 *  \param arg           [IN] Not currently used. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value
 *
 */
Int VpsHal_grpxInit(UInt32 numInstances,
                    const VpsHal_GrpxInstParams *initParams,
                     Ptr arg);


/**
 *  VpsHal_grpxDeInit
 *  \brief  GRPX HAL Exist Function
 *
 *  \param arg     [IN] Not Currently used. Meant for future purpose.
 *
 *  \return      Returns 0 on success else returns error value
 */
Int VpsHal_grpxDeInit(Ptr arg);


/**
 *  VpsHal_GrpxHandle VpsHal_grpxOpen
 *  \brief  GRPX HAL Open Function
 *
 *  This Function is used to return  grapchis handle . This function must be
 *  called before using any graphics. Handle returned from this function must
 *  be used in all other functions to set/get parameters correctly.
 *
 *  \param  grpxPort  [IN] request graphics port
 *
 *  \return handle    Returns GRPX instance handle on success else
 *                         returns NULL
 */
VpsHal_GrpxHandle VpsHal_grpxOpen(VpsHal_GrpxInst grpxPort);

/**
 *  VpsHal_grpxClose
 *  \brief  GRPX HAL Close Funcion
 *
 *  Function to close/release the grpahics handle.
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle    [IN] graphic handle
 *
 *  \return  Return 0 on sucess else returns error value
 */
Int VpsHal_grpxClose(VpsHal_GrpxHandle handle);

/**
 *  VpsHal_grpxSetFrameAttr
 *  \brief  GRPX HAL Set Frame Attribute Function
 *
 *     Function to set the frame display attribute in the overlay memory to
 *     create configuration descriptor used by the VPDMA.  This function should
 *     be called before any region attribute VPDMA transcation. The reset
 *     parameter is used to reset the corresponding graphics, even so, dispAttr
 *     should be still filled with the normal value to let the hardware work
 *     properly after reset. In order to get the offset and size(word) of this
 *     configuraiton in the overlay memory, call Grpx_getOvlyInfo.  The return
 *     information  is used for the VPDMA.
 *
 *  \param handle   [IN] Graphic port handle
 *  \param dispAttr [In] predefined memory location to store the frame
 *                       dispaly information
 *  \param reset    [IN] reset or not
 *  \param configOvlyPtr  [OUT] pointer to the configuration overlay memory,
 *                            it can not be null for GRPX module
 *
 *  \return    returns 0 on success or return error values
 */

Int VpsHal_grpxSetFrameAttr(VpsHal_GrpxHandle handle,
                            VpsHal_GrpxFrmDispAttr *dispAttr,
                            UInt32 reset,
                            Ptr configOvlyPtr);

/**
 *  VpsHal_grpxGetStatus
 *  \brief  GRPX HAL Get Status Function
 *
 *    Function to return the current GRPX interrupt error status and
 *         related error information
 *
 *  \param handle  [IN] GRPX port handle
 *  \param intStatus pointer to the structrues contained the all information
 *
 *  \return  Return 0 on success or return error values
 */
Int VpsHal_grpxGetStatus(VpsHal_GrpxHandle handle,
                         VpsHal_GrpxIntStatus  *intStatus);

/**
 *  VpsHal_grpxSetSc
 *  \brief  GRPX HAL Set scaler Function
 *
 *  Function to set the scaler coefficient and factors
 *
 *  \param handle    [IN] graphics port handle
 *  \param  scParams [IN] scaler factors
 *  \param configOvlyPtr [IN] predefined memory location to store the
 *  coefficients
 *
 *  \return  Return 0 on success or return error values
 */
Int VpsHal_grpxSetSc(VpsHal_GrpxHandle handle,
                     Vps_GrpxScParams *scParams,
                     Ptr configOvlyPtr );

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
                             Vps_GrpxScParams *scParams);

/**
 *  VpsHal_grpxRegionParamsCheck
 *  \brief  GRPX HAL Region Check Function
 *
 *  This function is to check the region display attribute to make sure this is
 *  correct
 *
 *  \param handle         [IN] Not currently used. Meant for future purpose.
 *  \param frmAttr        [IN] frame attribute
 *  \param regDispAttr    [IN] pointer to the structure containing the region
 *                             display attribute information
 *  \param scParams       [IN] scaling ration
 *  \param start          [IN] first region or not
 *
 *  \return   Return 0 on sucess else returns error value
 *
 */
 Int VpsHal_grpxRegionParamsCheck(VpsHal_GrpxHandle handle,
                                  VpsHal_GrpxFrmDispAttr  *frmAttr,
                                  Vps_GrpxRegionParams *regDispAttr,
                                  Vps_GrpxScParams *scParams,
                                  UInt32 start);
/**
 *  VpsHal_grpxRegionDispAttrSet
 *  \brief  GRPX HAL Set Region Attribute Function
 *
 *   Function is to set the region display attribute0 to the VPDMA descriptor
 *
 *  \param handle       [IN] Grpahics port handle.
 *  \param regDispAttr  [IN] pointer to the structure containing the region
 *                           display   attribute information
 *  \param descInfo     [OUT] pointer to the Vpdma region displaya attribute
 *                                structure
 *
 *  \return    Return 0 on sucess else returns error value
 */
Int VpsHal_grpxRegionDispAttrSet (VpsHal_GrpxHandle handle,
                                  Vps_GrpxRegionParams *regDispAttr,
                                  VpsHal_VpdmaRegionDataDescParams *descInfo);
/**
 *  VpsHaL_GrpxgetGrpxFrmAttrs
 *  \brief  GRPX HAL Get Frame Attribute Function
 *
 *  This function is to read the current frame display attributes of the
 *  assigned Graphic ported
 *
 *  \param handle     [IN] specify the GRPX port
 *  \param dispAttr   [OUT] pointer to store graphic frame display attributes
 *
 *  \return Return 0 on sucess else returns error value
*/
Int VpsHal_grpxGetFrmAttrs(VpsHal_GrpxHandle handle,
                           VpsHal_GrpxFrmDispAttr * dispAttrs);

/**
 *  VpsHal_grpxGetOvlyInfo
 *  \brief  GRPX HAL Get Overlay Function
 *
 *   Function to get the attribute hardware offset(128Bit for VPDMA transfer),
 *   size (word size for VPDMA transfer) and attribute word offset(to get the
 *   payload start address) in the current configruation descriptor overlay.
 *   These two information are used to fill the requirment in the VPMDA create
 *   configuration descriptor function.  Moreover, the destAdd(attribute
 *   offset) is also need to
 *
 *  \param handle      [IN]   grpx port handle
 *  \param ovlyType    [IN]   point to the attribue type
 *  \param ovlyInfo    [OUT]  overlay informaton for the assigned type
 *
 *  \return   Return 0 on sucess else returns error value
 */
Int VpsHal_grpxGetOvlyInfo(VpsHal_GrpxHandle handle,
                           VpsHal_GrpxVpdmaAttrOvly ovlyType,
                           VpsHal_GrpxVpdmaOvlyInfo *ovlyInfo);


#ifdef __cplusplus
extern "C"
}
#endif


 #endif //end of _VPSHAL_GRPX_h

