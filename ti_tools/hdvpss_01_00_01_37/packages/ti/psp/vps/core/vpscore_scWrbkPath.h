/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_scWrbkPath.h
 *
 *  \brief VPS Core header file for scalar path.
 *  This core is used by both capture as well as in M2M drivers.
 *  In capture mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_SCWRBKPATH_H
#define _VPSCORE_SCWRBKPATH_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Instance 0 - Write back scalar instance 0. */
#define VCORE_SWP_INST_WB2              (0u)
/**
 *  \brief Total number of instance. Change this macro accordingly when number
 *  of instance is changed.
 */
#define VCORE_SWP_NUM_INST              (1u)


#define VCORE_SWP_MAX_CHANNELS_PER_INST (64u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** struct Vcore_SwpParams
 * \brief Structure for configuring scalar parameters like scaling
 * ratio, scalar type etc.  These are the parameters apart from the format
 * structure specific to scalar */
typedef struct
{
    Vps_ScConfig        scCfg;
    /**< Scalar params */
    Vps_CropConfig      srcCropCfg;
    /**< Crop configuration */
    UInt32              srcWidth;
    /**< Source width for the scalar */
    UInt32              srcHeight;
    /**< Source height for the scalar */
} Vcore_SwpParams;

/** struct Vcore_SwpRtParams
 *   \brief Structure to suppport runtime params
 */
typedef struct
{
    Vps_FrameParams     *inFrmPrms;
    /**< Frame params for input frame
     *    Pass NULL if no change is required in input frame params
     */
    Vps_FrameParams     *outFrmPrms;
    /**< Frame params for output frame
     *    Pass NULL if no change is required in output frame params
     */
    Vps_CropConfig      *srcCropCfg;
    /**< Crop Runtime Configuration. If src frame params are changing crop
     *    configuration needs to be changed accordingly if required.
     *    Pass NULL if not change is required in cropping
     */
    Vps_ScRtConfig      *scRtCfg;
    /**< Scalar runtime configuration.
     *   Pass NULL if no change is required. */
}Vcore_SwpRtParams;

/**
 * struct Vcore_SwpInitParams
 *  \brief Structure containing instance specific parameters used at init call.
 */
typedef struct
{
    UInt32              instId;
    /**< Scalar Path Instance. */
    VpsHal_VpdmaChannel scCh;
    /**< Output channel for scalar */
    UInt32              maxHandle;
    /**< Maximum number of handles to be supported per instance. */
    VpsHal_Handle       scHandle;
    /**< Scalar handle */

} Vcore_SwpInitParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vcore_swpInit
 *  \brief Scalar path core init function.
 *  Initializes SWP core objects, allocates memory etc.
 *  This function should be called before calling any of SWP core API's.
 *
 *  \param numInstance  Number of instance objects to be initialized.
 *  \param initParams   Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstance.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_swpInit(UInt32 numInstance,
                    const Vcore_SwpInitParams *initParams,
                    Ptr arg);
/**
 *  Vcore_swpDeInit
 *  \brief Scalar path core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_swpDeInit(Ptr arg);
/**
 *  Vcore_swpGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_swpGetCoreOps(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_SCWRBKPATH_H */
