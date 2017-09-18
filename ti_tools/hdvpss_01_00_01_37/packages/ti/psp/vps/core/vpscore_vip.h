/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_vip.h
 *
 *  \brief VPS Core header file for VIP secondary and HDCOMP path.
 *  This core is used by both writeback capture as well as in M2M drivers.
 *  In capture mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_VIP_H
#define _VPSCORE_VIP_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/hal/vpshal_vps.h>


#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of channels supported for all handles.
 *  When the core is configured in capture mode, this is always 1.
 *  This is only used in mem-mem driver mode where software context makes sense.
 */
#define VCORE_VIP_MAX_CHANNELS          (64u)

/** \brief Instance ID for Secondary-VIP0 path. */
#define VCORE_VIP_INST_SEC0             (0u)
/** \brief Instance ID for Secondary-VIP1 path. */
#define VCORE_VIP_INST_SEC1             (1u)
/** \brief Instance ID for COMP-VIP0 path. */
#define VCORE_VIP_INST_COMP0            (2u)
/** \brief Instance ID for COMP-VIP1 path. */
#define VCORE_VIP_INST_COMP1            (3u)

/**
 *  \brief Total number of instance. Change this macro accordingly when number
 *  of instance is changed.
 */
#define VCORE_VIP_NUM_INST              (4u)

/** \brief Luma index to the buffer address array in FVID frame structure. */
#define VCORE_VIP_Y_IDX                 (0u)
/** \brief Chroma index to the buffer address array in FVID frame structure. */
#define VCORE_VIP_CBCR_IDX              (1u)
/** \brief RGB index to the buffer address array in FVID frame structure. */
#define VCORE_VIP_RGB_IDX               (0u)

/** \brief VIP index for SC module. */
#define VCORE_VIP_SC_IDX                (0u)
/** \brief VIP index for CSC module. */
#define VCORE_VIP_CSC_IDX               (1u)
/** \brief VIP index for CHR_DS module - No HAL is needed for this. */
#define VCORE_VIP_CHR_DS_IDX            (2u)

/** \brief Maximum number of HAL required by VIP - (CHR_DS not needed). */
#define VCORE_VIP_MAX_HAL               (2u)
/** \brief Maximum number of modules required by VIP. */
#define VCORE_VIP_MAX_MOD               (3u)

/* Control Command to allocate VIP resources in the VIP core. */
#define VCORE_IOCTL_VIP_ALLOC_PATH      (VCORE_CUSTOM_IOCTL_BASE + 0x0000u)
/* Control Command to allocate VIP resources in the VIP core. */
#define VCORE_IOCTL_VIP_FREE_PATH       (VCORE_CUSTOM_IOCTL_BASE + 0x0001u)
/* Control Command to figure out the state and calculate the configurations. */
#define VCORE_IOCTL_VIP_SET_CONFIG      (VCORE_CUSTOM_IOCTL_BASE + 0x0002u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_VipInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                  instId;
    /**< VIP Path Instance. */
    UInt32                  maxHandle;
    /**< Maximum number of handles/channels to be supported per instance. */
    VpsHal_VpsVipInst       vipInst;
    /**< VIP instance number to be passsed to VPS HAL for setting VIP mux. */
    VpsHal_Handle           halHandle[VCORE_VIP_MAX_HAL];
    /**< Array to pass the HAL handles required by core. */
    Vcore_VipInSrc          vipInSrc;
    /**< VIP input source for this instance. This will be passed to
         the VIP resource manager */
    UInt32                  numVipOutSteams;
    /**< Number of VIP output formats */
} Vcore_VipInitParams;

/**
 *  struct Vcore_VipParams
 *  \brief VIP config parameters.
 */
typedef struct
{
    UInt32                  memType;
    /**< VPDMA Memory type. For valid values see #Vps_VpdmaMemoryType. */
    FVID2_Format            fmt;
    /**< FVID frame format. */
    Vps_ScConfig            scCfg;
    /**< Scalar parameters */
    Vps_CropConfig          cropCfg;
    /**< Cropping parameters */
    UInt32                  srcWidth;
    /**< Source image Width. */
    UInt32                  srcHeight;
    /**< Source image Height. */
    UInt32                  isScReq;
    /**< Flag to indicate whether scalar is needed for this stream
         in the channel or not. VIP can output multiple streams. Out of
         these streams, this flag indicates which all streams
         needs scalar. Bypass flag in scalar just informs whether it
         is bypassed or not.
         Caution: When scalar is used in multiple streams, it
         must be enabled/disabled in all the streams and output
         size in the fmt must be same in all the streams. Core
         does not check for this error. */
} Vcore_VipParams;

/**
 *  struct Vcore_VipRtParams
 *  \brief VIP runtime configuration parameters.
 */
typedef struct
{
    Vps_FrameParams        *inFrmPrms;
    /**< Frame params for input frame as passed by application.
         Pass NULL if no change is required. */
    Vps_FrameParams        *outFrmPrms;
    /**< Frame params for VIP output frame.
         Pass NULL if no change is required or this output is not used
         by an instance. */
    Vps_CropConfig         *scCropCfg;
    /**< Scalar crop configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_ScRtConfig         *scRtCfg;
    /**< Scalar runtime configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    UInt32                  isDeinterlacing;
    /**< Flag to represent whether DEI is deinterlacing or not. This is
         used when input to the VIP is from the DEI core. This is because
         when DEI is deinterlacing, the height will become twice as compared
         to the height present in inFrmPrms. For other drivers, this should
         be set to FALSE. */
} Vcore_VipRtParams;


/* ========================================================================== */
/*                          Function Declarations                             */
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
 *                      structure of size numInstance.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_vipInit(UInt32 numInst,
                    const Vcore_VipInitParams *initPrms,
                    Ptr arg);

/**
 *  Vcore_vipDeInit
 *  \brief VIP core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_vipDeInit(Ptr arg);

/**
 *  Vcore_vipGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_vipGetCoreOps(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_VIP_H */
