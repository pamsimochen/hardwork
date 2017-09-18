/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_M2M_COMMON
 * \defgroup VPS_DRV_FVID2_COMP_API M2M COMP API
 *
 *  \par Caution
 *       The interfaces defined in this file is bound to change.
 *       Kindly treat the interfaces as work in progress.
 *       Release notes/user guide list the additional limitation/restriction
 *       of this module/interfaces.
 *
 * @{
 */

/**
 *  \file vps_m2mComp.h
 *
 *  \brief COMP (video + graphics blending and scaling) based memory to memory driver API
 */

#ifndef _VPS_M2MCOMP_H
#define _VPS_M2MCOMP_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*
 *  Blender based (COMP) M2M driver IOCTLs.
 */
/* None */

/*
 *  Macros for different driver instance numbers to be passed as instance ID
 *  at the time of driver create.
 *  Note: These are read only macros. Don't modify the value of these macros.
 */
/*
 *  ___________________________________________________________________________________
 *  INPUT PATH               |  MODULES IN PATH             | OUTPUT PATH/S            |
 *  _________________________|______________________________|__________________________|
 *  BYPASS 0                 |  VCOMP, EDE, CRPOC/ CSC      |  CSC + SC5               |
 *  BYPASS 1                 |  CIG, BLENDER 1 or 3         |  VIP0 (SC3, CHR_DS)      |
 *  MAIN DEI_H               |                              |  VIP1 (SC4, CHR_DS)      |
 *  DEI                      |                              |  VIP0(1 o/p)+VIP1(2 o/p) |
 *  +GRAPHICS                |                              |  VIP0+VIP1               |
 *  _________________________|______________________________|__________________________|
 */
/** \brief BP0 path-GRPX-HDMI Blender-SC5 writeback memory driver instance number. */
#define VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB          (0u)
/** \brief BP1 path-GRPX-HDMI Blender-SC5 writeback memory driver instance number. */
#define VPS_M2M_INST_COMP_BP1_GRPX_CSC_BLND1_SC5_WB          (1u)
/** \brief DEI HQ-GRPX-HDMI Blender-SC5 writeback memory driver instance number. */
#define VPS_M2M_INST_COMP_MAIN_DEIH_GRPX_BLND1_SC5_WB    (2u)
/** \brief DEI-GRPX-Blender-VIP0 memory driver instance number. */
#define VPS_M2M_INST_COMP_DEI_GRPX_BLND3_VIP0_WB         (3u) /*??needed*/
/** \brief DEI-GRPX-Blender-VIP1 memory driver instance number. */
#define VPS_M2M_INST_COMP_DEI_GRPX_BLND3_VIP1_WB         (4u)
/** \brief DEI-GRPX-Blender-VIP0 and VIP1 memory driver instance number. */
#define VPS_M2M_INST_COMP_DEI_GRPX_BLND3_VIP0_VIP1_WB    (5u)
/** \brief BP0-GRPX-Blender-VIP0 memory driver instance number. */
#define VPS_M2M_INST_COMP_BP0_GRPX_BLND3_VIP0_WB         (6u) /*??needed*/
/** \brief BP0-GRPX-Blender-VIP1 memory driver instance number. */
#define VPS_M2M_INST_COMP_BP0_GRPX_BLND3_VIP1_WB         (7u)
/** \brief BP0-GRPX-Blender-VIP0 and VIP1 memory driver instance number. */
#define VPS_M2M_INST_COMP_BP0_GRPX_BLND3_VIP0_VIP1_WB    (8u)
/** \brief BP1-GRPX-Blender-VIP0 memory driver instance number. */
#define VPS_M2M_INST_COMP_BP1_GRPX_BLND3_VIP0_WB         (9u) /*??needed*/
/** \brief BP1-GRPX-Blender-VIP1 memory driver instance number. */
#define VPS_M2M_INST_COMP_BP1_GRPX_BLND3_VIP1_WB         (10u)
/** \brief BP1-GRPX-Blender-VIP0 and VIP1 memory driver instance number. */
#define VPS_M2M_INST_COMP_BP1_GRPX_BLND3_VIP0_VIP1_WB    (11u)

/**
 *  \brief Maximum number of M2M COMP driver instances.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_COMP_INST_MAX            (12u)

/**
 *  \brief Maximum number of handles supported by M2M COMP driver per
 *  driver instance.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_COMP_MAX_HANDLE_PER_INST (4u) //??

/**
 *  \brief Maximum number of channels that can be submitted in one request
 *  call per handle. This macro is used in allocation of memory pool objects and
 *  is assumed that if more that one handle is opened per instance, then the
 *  channel memory will be shared across handles.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_COMP_MAX_CH_PER_INST     (4u) //??

/**
 *  \brief Maximum number of channels that can be submitted.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_COMP_MAX_CH              (64u) //??



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
* Structure declarations for FVID2 API of this driver, namely:
* CreateArgs, CreateStatusArgs: ChParams
* deleteArgs
* control: cmdArgs, cmdStatusArgs
* runtimePrms
**/


/**
 *  struct Vps_M2mCompChParams
 *  \brief COMP M2M channel parameter structure to be passed to the
 *  driver at the time of mem to mem driver create call.
 */
typedef struct
{
    /****** input path related parameters :video ==> BP0/BP1/DEI_H/DEI; Graphics==> GRPX1/2/3 *****/
    FVID2_Format            inFmt;
    /**< Frame format for the input frame to the COMP. */
    UInt32                  inMemType;
    /**< VPDMA Memory type for the COMP input frame.
         For valid values see #Vps_VpdmaMemoryType. */

    Vps_GrpxCreateParams    *grpxPrms;
    /**< Graphics parameters . */

    /** For DEI input path, DEI and SC are assumed to be bypassed as no known use case
        for this driver does deinterlacing or pre-scaling after video composition*/
    UInt32                  drnEnable;
    /**< Enables/disables the DRN module in the DEI path. */
    Vps_ScConfig           *deiScCfg;
    /**< Pointer to the scalar parameters like crop and scaler type for the
         scalar in DEI path. */
    Vps_CropConfig         *deiScCropCfg;
    /**< Pointer to the cropping configuration for the DEI scalar. */


    /***** parameters for modules in path : VCOMP, EDE, CRPOC/CSC, BLENDER 1,3 *****/
    UInt32                  vcompEnable;
    /**< Enable VCOMP, this is for selection of VCOMP-EDE-CPROC path versus CSC path. VCOMP is always bypassed */
    UInt32                  edeEnable;
    /**< Enables/disables the DRN module in the COMP path. */

    //Vps_CompConfig        *compCfg;
    /**< Pointer to the compositor/Blender configuration used for
      *  COMP  drivers.
      *  This parameter can be set to NULL to use default COMP configuration */

    /****** output path related parameters : SC5/VIP0/VIP1 *****/
    FVID2_Format           *outFmtSc5;
    /**< Frame format for the output frame from COMP-Scalar5 output. */
    UInt32                  outMemTypeSc5;
    /**< VPDMA Memory type for the COMP-Scalar output frame.
         For valid values see #Vps_VpdmaMemoryType. */

    FVID2_Format           *outFmtVip0;
    /**< Frame format for the output frame from COMP-VIP output - non-scaled. */
    UInt32                  outMemTypeVip0;
    /**< VPDMA Memory type for the COMP-VIP output frame- non-scaled.
         For valid values see #Vps_VpdmaMemoryType. */
    FVID2_Format           *outFmtVip0Sc;
    /**< Frame format for the output frame from COMP-VIP scaled output. */
    UInt32                  outMemTypeVip0Sc;
    /**< VPDMA Memory type for the COMP-VIP output frame.
         For valid values see #Vps_VpdmaMemoryType. */
    FVID2_Format           *outFmtVip1Sc;
    /**< Frame format for the output frame from COMP-VIP scaled output. */
    UInt32                  outMemTypeVip1Sc;
    /**< VPDMA Memory type for the COMP-VIP output frame.
         For valid values see #Vps_VpdmaMemoryType. */


    Vps_ScConfig           *sc5Cfg;
    /**< Pointer to the scalar parameters like crop and scaler type for the
         write-back scalar: SC5 */
    Vps_CropConfig         *sc5CropCfg;
    /**< Pointer to the cropping configuration for the write-back scalar: SC5 */
    Vps_ScConfig           *vip0ScCfg;
    /**< Pointer to the scalar parameters like crop and scaler type for the
         VIP scalar. */
    Vps_CropConfig         *vip0CropCfg;
    /**< Pointer to the cropping configuration for the VIP scalar. */
    Vps_ScConfig           *vip1ScCfg;
    /**< Pointer to the scalar parameters like crop and scaler type for the
         VIP scalar. */
    Vps_CropConfig         *vip1CropCfg;
    /**< Pointer to the cropping configuration for the VIP scalar. */
} Vps_M2mCompChParams;


/**
 *  struct Vps_M2mCompCreateParams
 *  \brief COMP M2M create parameter structure to be passed to the
 *  driver at the time of mem to mem driver create call.
 */
typedef struct
{
    UInt32                  mode;
    /**< Mode to indicate whether configuration is required per handle or
         per channel. For valid values see #Vps_M2mMode. */
    UInt32                  numCh;
    /**< Number of channels to be supported by handle. */

    const Vps_M2mCompChParams   *chParams;
    /**< Pointer to an array of channel params.  Array will contain only one
         element if the mode of the handle is configuration per handle
         else it will contain elements equal to the  number of channels
         that requires to be supported by a handle. */
    Vps_GrpxParamsList        *grpxPrms;
    /**< is used to define the parameters for all regions in graphics path. */

} Vps_M2mCompCreateParams;

/**
 *  sturct Vps_M2mCompCreateStatus
 *  \brief Create status for the M2M COMP create call. This should be
 *  passed to the create function as a create status argument while creating
 *  the driver.
 */
typedef struct
{
    Int32                   retVal;
    /**< Return value of create call. */
    UInt32                  maxHandles;
    /**< Maximum number of handles supported by driver per instance. */
    UInt32                  maxChPerHandle;
    /**< Maximum number of channels that can be supported/submitted per
         handle. Note that the the maximum number of channels will be
         shared between handles if more than one handle is opened.
         This is to avoid unnecessary wastage memory allocation. */
    UInt32                  maxSameChPerRequest;
    /**< Maximum number of same channels that can be submitted for processing
         per request. */
    UInt32                  maxReqInQueue;
    /**< Maximum number of request per driver handle that can be submitted
         for processing without having to dequeue the completed requests. */
    Vps_GrpxCreateStatus    grpxStatus;
    /**< Status of GRPX */
} Vps_M2mCompCreateStatus;



/**
 *  struct Vps_M2mCompRtParams
 *  \brief Run time configuration structure for the COMP memory to memory
 *  driver. This needs to be passed along with process list to
 *  update any supported run time parameters.
 */
typedef struct
{

    Vps_LayoutId           *layoutId;
    /**< ID of the layout to be selected. This should be a valid layout ID
         as returned by create multi window layout IOCTL. When layout ID is
         NULL, the driver will ignore this runtime parameter
         and continue processing the submitted request. */

    Vps_CropConfig         *Sc5CropCfg;
    /**< Scalar crop configuration for writeback scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_ScRtConfig         *sc5RtCfg;
    /**< Scalar runtime configuration for COMP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */

    Vps_CropConfig         *vipSc3CropCfg;
    /**< Scalar crop configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_ScRtConfig         *vipSc3RtCfg;
    /**< Scalar runtime configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */

    Vps_CropConfig         *vipSc4CropCfg;
    /**< Scalar crop configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_ScRtConfig         *vipSc4RtCfg;
    /**< Scalar runtime configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
#if 0 //? do we need any GRPX RtPrms
    Vps_GrpxRtParams    ;
    Vps_GrpxRtList        *grpxRtCfg;
    /**< Graphics runtime configuration.
         Pass NULL if no change is required. */
#endif
} Vps_M2mCompRtParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_M2MCOMP_H */

  /*  @}  */
