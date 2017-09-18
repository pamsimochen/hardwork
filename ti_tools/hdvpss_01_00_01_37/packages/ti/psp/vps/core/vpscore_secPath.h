/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_secPath.h
 *
 *  \brief VPS Core header file for secondary path involving chroma up sampler.
 *  This core is used by both display as well as in M2M drivers.
 *  In display mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_SECPATH_H
#define _VPSCORE_SECPATH_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Instance 0 - Secondary path 0. */
#define VCORE_SEC_INST_0                (0u)
/** \brief Instance 1 - Secondary path 1. */
#define VCORE_SEC_INST_1                (1u)
/**
 *  \brief Total number of instance. Change this macro accordingly when number
 *  of instance is changed.
 */
#define VCORE_SEC_NUM_INST              (2u)

#define VCORE_SEC_MAX_CHANNELS_PER_INST (64u)

/** \brief Number of VPDMA channels per instance. */
#define VCORE_SEC_VPDMA_CH_PER_INST     (2u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_SecInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32              instId;
    /**< Secondary path Instance. */
    VpsHal_VpdmaChannel vpdmaCh[VCORE_SEC_VPDMA_CH_PER_INST];
    /**< VPDMA channel numbers for the 422T/420T  path. */
    UInt32              maxHandle;
    /**< Maximum number of handles to be supported per instance. */
    VpsHal_Handle       chrusHandle;
    /**< Handle to chroma up sampler HAL. */
    VpsHal_VpsClkcModule module;
    /**< Module enum for initializing the clock */
    VpsHal_VpdmaPath     vpdmaPath;
    /**< VPDMA Path */
} Vcore_SecInitParams;

/**
 * struct Vcore_SecRtParams
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
} Vcore_SecRtParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vcore_secInit
 *  \brief Secondary path core init function.
 *  Initializes SEC core objects, allocates memory etc.
 *  This function should be called before calling any of SEC core API's.
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
Int32 Vcore_secInit(UInt32 numInstance,
                   const Vcore_SecInitParams *initParams,
                   Ptr arg);
/**
 *  Vcore_secDeInit
 *  \brief Secondary path core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_secDeInit(Ptr arg);
/**
 *  Vcore_secGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_secGetCoreOps(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_SECPATH_H */
