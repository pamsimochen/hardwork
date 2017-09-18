/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_cscCig.h
 *
 *  \brief VPS Core header file for CSC display path above CIG and blenders.
 *  This core is used by both display as well as in M2M drivers.
 *  In display mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_CSC_CIG_H
#define _VPSCORE_CSC_CIG_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Instance 0 - CSC CIG display path. */
#define VCORE_CSC_CIG_INST_0                (0u)
/**
 *  \brief Total number of instance. Change this macro accordingly when number
 *  of instance is changed.
 */
#define VCORE_CSC_CIG_NUM_INST              (1u)

#define VCORE_CSC_CIG_MAX_CHANNELS_PER_INST (64u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_CscCigInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32              instId;
    /**< CSC CIG path Instance. */
    UInt32              maxHandle;
    /**< Maximum number of handles to be supported per instance. */
    VpsHal_Handle       cscHandle;
    /**< Handle to CSC HAL. */
    VpsHal_Handle       cigHandle;
    /**< Handle to CIG HAL. */
    VpsHal_Handle           blendHdmiHandle;
    /**< Handle of HDMI Blender HAL.  This will be used to configure Blender. */
    VpsHal_Handle           cscWrbkHandle;
    /**< Handle of CSC HAL.  This will be used to configure Writeback CSC. */
} Vcore_CscCigInitParams;

/**
 * struct Vcore_CscCigRtParams
 * \brief Strucutre to support the run time parameters
 */
typedef struct
{
    Vps_FrameParams        *inFrmPrms;
    /**< Frame params for input frame.
         Pass NULL if no change is required. */
    Vps_PosConfig          *posCfg;
    /**< Runtime position configuration containing startX and startY.
         Pass NULL if no change is required. */
} Vcore_CscCigRtParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vcore_cscCigInit
 *  \brief CSC CIG path core init function.
 *  Initializes CSC_CIG core objects, allocates memory etc.
 *  This function should be called before calling any of CSC_CIG core API's.
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
Int32 Vcore_cscCigInit(UInt32 numInstance,
                   const Vcore_CscCigInitParams *initParams,
                   Ptr arg);
/**
 *  Vcore_cscCigDeInit
 *  \brief CSC CIG path core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_cscCigDeInit(Ptr arg);
/**
 *  Vcore_cscCigGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_cscCigGetCoreOps(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_CSC_CIG_H */
