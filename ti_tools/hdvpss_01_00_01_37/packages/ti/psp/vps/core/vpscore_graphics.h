/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_graphics.h
 *
 *  \brief VPS Core header file for graphics path
 *  This core is used by both display as well as in M2M drivers.
 *  In display mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_GRAPHICS_H
#define _VPSCORE_GRAPHICS_H

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

/**
 *  \brief Maximum number of sub context supported per channel.
 *  When the core is configured in display mode, this is always 1.
 *  This is only used in mem-mem driver mode where software context makes sense.
 */
#define VCORE_GRPX_MAX_CHANNELS        (16u)

/** \brief Instance 0 - GRPX0. This is read only MACRO*/
#define VCORE_GRPX_INST_0              (VPS_DISP_INST_GRPX0)
/** \brief Instance 1 - GRPX1. This is read only MACRO*/
#define VCORE_GRPX_INST_1              (VPS_DISP_INST_GRPX1)
/** \brief Instance 1 - GRPX2. This is read only MACRO*/
#define VCORE_GRPX_INST_2              (VPS_DISP_INST_GRPX2)

/**
 *  \brief Total number of instance. Change this macro accordingly when number
 *  of instance is changed.
 */
#define VCORE_GRPX_NUM_INST            (VPS_DISP_GRPX_MAX_INST)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_GrpxRtParams
 *  \brief Structure containing runtime params used by the core
 */

typedef struct
{
    Vps_GrpxRtList          *rtList;
    /**< RT parameters sharing by the all regions, */
    Vps_GrpxRtParams        *rtParams;
    /**< RT paramters of each regions regions*/
}Vcore_GrpxRtParams;

/**
 *  struct Vcore_GrpxInitParams
 *  \brief Structure containing instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                   instId;
    /**< Graphics Instance. */
    VpsHal_VpdmaChannel      chGrpx;
    /**< VPDMA channel number for the grpx path. */
    VpsHal_VpdmaChannel      chSten;
    /**< VPDMA channel number for the stenciling path, which should
         be associated with grpx path. */
    VpsHal_VpdmaChannel      chClut;
    /**< VPDMA channel number for the CLUT path, which should be
         associated with grpx path. */
    VpsHal_VpdmaPath          ptGrpx;
    /*VPDMA GRPX Data path name*/
    VpsHal_VpdmaPath          ptSten;
    /*VPDMA GRPX stenciling data path name*/
    Vem_ErrorEvent           event;
    /** < error event associated to the grpx path*/
    UInt32                   maxHandle;
    /**< Maximum number of handles/channels to be supported per instance. */
    VpsHal_GrpxHandle        grpxHandle;
    /**< grpx HAL handle*/
} Vcore_GrpxInitParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 *  Vcore_grpxInit
 *  \brief Graphics Core init function.
 *  Initializes GRPX core objects, allocates memory etc.
 *  This function should be called before calling any of GRPX core API's.
 *
 *  \param numInst      [IN] Number of instance objects to be initialized
 *  \param initParams   [IN] Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances.
 *  \param arg          [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */

Int32 Vcore_grpxInit(UInt32 numInst,
                     const Vcore_GrpxInitParams *initParams,
                     Ptr arg);
/**
 *  Vcore_grpxDeInit
 *  \brief Graphics Core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */

Int32 Vcore_grpxDeInit(Ptr arg);

/**
 *  Vcore_grpxGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */

const Vcore_Ops *Vcore_grpxGetCoreOps(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_GRAPHICS_H */

