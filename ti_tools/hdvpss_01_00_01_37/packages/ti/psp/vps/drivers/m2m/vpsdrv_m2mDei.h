/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDei.h
 *
 *  \brief This file implements the memory to memory driver for the
 *  DEI and DEI HQ with outputs through VIP0/1 and/or DEI Write back paths.
 *
 */

#ifndef _VPSDRV_M2MDEI_H
#define _VPSDRV_M2MDEI_H

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

/** \brief Core index for DEI core. */
#define VPSMDRV_DEI_DEI_CORE_IDX        (0u)
/** \brief Core index for DEI writeback core. */
#define VPSMDRV_DEI_DWP_CORE_IDX        (1u)
/** \brief Core index for VIP core. */
#define VPSMDRV_DEI_VIP_CORE_IDX        (2u)
/* Note: If a new core is to be added into this that has a scalar in it, refer
 * to vps_m2mDei.h:
 * VPS_M2M_DEI_SCALAR_ID_DEI_SC, VPS_M2M_DEI_SCALAR_ID_VIP_SC.
 * The new scalarId corresponding to this core needs to be added at the end
 * and the new core with that scalar ID must be added at the end here. This
 * same ordering is assumed within the code.
 */
/** \brief Maximum number of core required by driver. */
#define VPSMDRV_DEI_MAX_CORE            (3u)
/** \brief Maximum number resources required by driver. */
#define VPSMDRV_DEI_MAX_RESR            (4u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsMdrv_DeiInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                      drvInstId;
    /**< Driver instance ID. */
    VpsHal_VpsSecInst           secMuxInstId;
    /**< Secondary path mux instance ID. */
    Vrm_Resource                resrcId[VPSMDRV_DEI_MAX_RESR];
    /**< DEI HQ/DEI, DWP, VIP core resource ID. */
    UInt32                      coreInstId[VPSMDRV_DEI_MAX_CORE];
    /**< DEI HQ/DEI, DWP, VIP core instance number/ID. */
    const Vcore_Ops            *coreOps[VPSMDRV_DEI_MAX_CORE];
    /**< DEI HQ/DEI, DWP, VIP core function pointers. */
} VpsMdrv_DeiInitParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsMdrv_deiInit
 *  \brief DEI M2M driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of FVID2 APIs for this
 *  driver.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initPrms     Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_deiInit(UInt32 numInst,
                      const VpsMdrv_DeiInitParams *initPrms,
                      Ptr arg);

/**
 *  VpsMdrv_deiDeInit
 *  \brief DEI M2M driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_deiDeInit(Ptr arg);


#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_M2MDEI_H */
