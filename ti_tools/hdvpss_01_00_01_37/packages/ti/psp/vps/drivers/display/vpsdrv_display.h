/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_display.h
 *
 *  \brief VPS display driver for the two bypass paths and the SD display path.
 *  This file exposes the APIs of the VPS display driver.
 *
 */

#ifndef _VPSDRV_DISPLAY_H
#define _VPSDRV_DISPLAY_H

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
 *  struct VpsDdrv_InitPrms
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                  drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource            resrcId;
    /**< Resource ID. */
    Dc_NodeNum              dcNodeNum;
    /**< Node number for registering to display controller. */
    UInt32                  coreInstId;
    /**< Core instance number/ID. */
    const Vcore_Ops        *coreOps;
    /**< Core function pointers. */
} VpsDdrv_InitPrms;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsDdrv_init
 *  \brief Display driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of display driver API.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initPrms     Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 VpsDdrv_init(UInt32 numInst,
                   const VpsDdrv_InitPrms *initPrms,
                   Ptr arg);

/**
 *  VpsDdrv_deInit
 *  \brief Display driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 VpsDdrv_deInit(Ptr arg);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSDRV_DISPLAY_H */
