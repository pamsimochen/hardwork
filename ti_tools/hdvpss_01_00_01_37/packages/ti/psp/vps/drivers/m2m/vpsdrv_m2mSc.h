/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mSc.h
 *
 *  \brief This file implements the memory to memory driver for the
 *  two bypass paths BP0, BP1 and Secondary path SEC0 to SC5 or
 *  Secondary path SEC0/1 to VIP0/1 SC (SC3/4).
 *
 */

#ifndef _VPSDRV_M2MSC_H
#define _VPSDRV_M2MSC_H

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
#define VPSMDRV_SC_IN_CORE_IDX          (0u)
/** \brief Core index for DEI writeback core. */
#define VPSMDRV_SC_OUT_CORE_IDX         (1u)
/** \brief Maximum number of core required by driver. */
#define VPSMDRV_SC_MAX_CORE             (2u)

/** \brief Maximum number resources required by driver. */
#define VPSMDRV_SC_MAX_RESR             (4u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * struct VpsMdrv_ScInitParams
 * \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                      drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource                resrcId[VPSMDRV_SC_MAX_RESR];
    /**< ID of the resources required for this instance of the driver. */
    UInt32                      inCoreInstId;
    /**< Input core instance number/ID. */
    UInt32                      outCoreInstId;
    /**< Output core instance number/ID. */
    const Vcore_Ops            *inCoreOps;
    /**< Input core function pointers. */
    const Vcore_Ops            *outCoreOps;
    /**< Output core function pointers. */
} VpsMdrv_ScInitParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsMdrv_scInit
 *  \brief Scalar M2M driver init function.
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
Int32 VpsMdrv_scInit(UInt32 numInst,
                     const VpsMdrv_ScInitParams *initPrms,
                     Ptr arg);

/**
 *  VpsMdrv_scDeInit
 *  \brief Scalar M2M driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_scDeInit(Ptr arg);


#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_M2MSC_H */
