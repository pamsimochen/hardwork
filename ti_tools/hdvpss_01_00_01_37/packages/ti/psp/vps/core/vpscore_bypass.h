/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_bypass.h
 *
 *  \brief VPS Core header file for 422 bypass path.
 *  This core is used by both display as well as in M2M drivers.
 *  In display mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_BYPASS_H
#define _VPSCORE_BYPASS_H

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

/** \brief Instance ID for BP0 path. */
#define VCORE_BP_INST_0                 (0u)
/** \brief Instance ID for BP1 path. */
#define VCORE_BP_INST_1                 (1u)
/**
 *  \brief Maximum number of instance. Change this macro accordingly when
 *  number of instance is changed.
 */
#define VCORE_BP_INST_MAX               (2u)

/** \brief Maximum number of handles supported per bypass path instance. */
#define VCORE_BP_MAX_HANDLES            (4u)

/**
 *  \brief Maximum number of channels supported for all handles.
 *  When the core is configured in display mode, this is always 1.
 *  This is only used in M2M driver mode where software context makes sense.
 */
#define VCORE_BP_MAX_CHANNELS           (64u)

/**
 *  \brief Maximum number of multiwindow settings that can be set for an
 *  instance. This is used in dynamic mosaic layout change.
 *  Note: When mosaic layout is supported in M2M mode, this should be
 *  changed to at least a minimum of VCORE_BP_MAX_CHANNELS count, so that
 *  each channel could get one mosaic configuration memory.
 */
#define VCORE_BP_MAX_MULTIWIN_SETTINGS  (16u)

/** \brief BP VPDMA channel index for 422p path. */
#define VCORE_BP_422P_IDX               (0u)
/** \brief Maximum number of VPDMA channels requried by BP. */
#define VCORE_BP_MAX_VPDMA_CH           (1u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_BpInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                  instId;
    /**< Bypass Path Instance. */
    UInt32                  maxHandle;
    /**< Maximum number of handles/channels to be supported per instance. */
    VpsHal_VpdmaChannel     vpdmaCh[VCORE_BP_MAX_VPDMA_CH];
    /**< Array to pass the VPDMA channel number required by core. */
    VpsHal_VpdmaPath        vpdmaPath;
    /**< VPDMA Path name. */
} Vcore_BpInitParams;

/**
 *  struct Vcore_BpRtParams
 *  \brief Bypass core runtime parameter structure.
 */
typedef struct
{
    Vps_FrameParams        *inFrmPrms;
    /**< Frame params for input frame.
         Pass NULL if no change is required. */
    Vps_PosConfig          *posCfg;
    /**< Runtime position configuration containing startX and startY.
         Pass NULL if no change is required. */
} Vcore_BpRtParams;


/* ========================================================================== */
/*                          Function Declarations                             */
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
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_bpInit(UInt32 numInst,
                   const Vcore_BpInitParams *initPrms,
                   Ptr arg);

/**
 *  Vcore_bpDeInit
 *  \brief Bypass path core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_bpDeInit(Ptr arg);

/**
 *  Vcore_bpGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_bpGetCoreOps(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_BYPASS_H */
