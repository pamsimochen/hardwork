/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_graphics.h
 *
 *  \brief VPS graphics display driver for the three independent GRPX path.
 *  This file exposes the APIs of the VPS Display graphics driver.
 *
 */

#ifndef _VPSDRV_GRAPHICS_H
#define _VPSDRV_GRAPHICS_H

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

/* None */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsDdrv_GrpxInitParams
 *  \brief Structure containing instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                  drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource            resourceId;
    /**< Resource ID. */
    Dc_NodeNum              dcNodeNum;
    /**< Node number for registering to display controller. */
    UInt32                  coreInstId;
    /**< Core instance number/ID. */
    const Vcore_Ops        *coreOps;
    /**< Core function pointers. */
} VpsDdrv_GrpxInitParams;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsDdrv_grpxInit
 *  \brief Graphics driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of graphics driver API.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initParams   [IN] Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances.
 *  \param arg          [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 VpsDdrv_grpxInit(UInt32 numInst, const VpsDdrv_GrpxInitParams *initParams,
                   Ptr arg);

/**
 *  VpsDdrv_grpxDeInit
 *  \brief Graphics driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 VpsDdrv_grpxDeInit(Ptr arg);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSDRV_GRAPHICS_H */

