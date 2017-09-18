/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

 /**
 *
 *  \brief This file implements the memory to memory driver for the
 *  COMP module based blending video, graphics and scaling
 *
 */
 #ifndef _VPSDRV_M2MCOMP_H
 #define _VPSDRV_M2MCOMP_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
/* none */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Core index for Input Mosaic Video core. (BP0/1, DEIH/DEI) */
#define VPSMDRV_COMP_CORE_IN_VIDEO_IDX        (0u)
/** \brief Core index for Input Graphics core. (GRPX0/1/2) */
#define VPSMDRV_COMP_CORE_IN_GRPX_IDX         (1u)
/** \brief Core index for COMP OUTPUT core 0. (SC5WRBK/VIP0/VIP1 */
#define VPSMDRV_COMP_CORE_OUT_WRBK0_IDX       (2u)
/** \brief Core index for COMP OUTPUT core 1. (VIP0/VIP1 */
#define VPSMDRV_COMP_CORE_OUT_WRBK1_IDX       (3u)
/** \brief Core index for COMP path core. (VCOMP_EDE_CPROC_CIG/ CSC_CIG) */
#define VPSMDRV_COMP_CORE_PROCESS_PATH_IDX    (4u) //??required OR can be controlled with Dctrl fcns
/** \brief Maximum number of core required by driver. */
#define VPSMDRV_COMP_MAX_CORE                 (5u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */
/**
 * struct Comp_initParams_t
 * \brief Init params structure for the driver.
 */
 typedef struct Comp_initParams_t
 {
    UInt32                      drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource                resrcId[VPSMDRV_COMP_MAX_CORE];
    /**< DEI, DWP, VIP core resource ID. */
    UInt32                      coreInstId[VPSMDRV_COMP_MAX_CORE];
    /**< DEI, DWP, VIP core instance number/ID. */
    const Vcore_Ops            *coreOps[VPSMDRV_COMP_MAX_CORE];
    /**< DEI, DWP, VIP core function pointers. */
 }VpsMdrv_CompInitParams; /**< Typedef for struct Comp_initParams_t. */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 *  VpsMdrv_CompDeInit
 *  \brief Comp/blender writeback memory-memory driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 VpsMdrv_compDeInit(Ptr arg);

/**
 *  VpsMdrv_CompInit
 *  \brief Scalar writeback memory-memory driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of FVID2 APIs for this
 *  driver.
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
Int32 VpsMdrv_compInit(UInt32 numInstances, const VpsMdrv_CompInitParams *initParams, Ptr arg);

#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_M2MCOMP_H */
