/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_cproc.h
 *
 *  \brief VPS Cproc HAL header file
 *  This file exposes the HAL APIs of the VPS Cproc.
 *
 */

#ifndef VPS_CPROCHAL_H
#define VPS_CPROCHAL_H

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

/** \brief  CPROC instance */
#define VPSHAL_CPROC_INST_0             (0u)
/** \brief CPROC maximum number of instances */
#define VPSHAL_CPROC_MAX_INST           (1u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_CprocInstParams
 *  \brief Structure containing Instance Specific Parameters
 *
 *  instId        Instance Identifier
 *  baseAddress   Physical register start address for this instance
 */
typedef struct
{
    UInt32             instId;
    /**< Cproc Instance */
    UInt32             baseAddress;
    /**< Base address of the instance */
} VpsHal_CprocInstParams;


typedef struct
{
    UInt32 width;
    UInt32 height;
    UInt32 bypass;
} VpsHal_CprocConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 *  VpsHal_cprocInit
 *  \brief CPROC HAL init function.
 *
 *  Initializes CPROC objects, gets the register overlay offsets for CPROC
 *  registers.
 *  This function should be called before calling any of CPROC HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of CPROC HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_cprocInit(UInt32 numInstances,
                     VpsHal_CprocInstParams *cprocInstParams,
                     Ptr arg);



/**
 *  VpsHal_cprocDeInit
 *  \brief CPROC HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_cprocDeInit(Ptr arg);


/**
 *  VpsHal_cprocOpen
 *  \brief Returns the handle to the requested CPROC instance.
 *
 *  This function should be called prior to calling any of the CPROC HAL
 *  configuration APIs to get the instance handle.
 *
 *  Cproc_init should be called prior to this.
 *
 *  \param cprocInst Requested CPROC instance.
 *
 *  \return          Returns CPROC instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_cprocOpen(UInt32 cprocInst);



/**
 *  VpsHal_cprocClose
 *  \brief Closes the CPROC HAL instance
 *
 *  Cproc_init and Comp_open should be called prior to this.
 *
 *  \param handle Valid handle returned by Cproc_open function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_cprocClose(VpsHal_Handle handle);


Int VpsHal_cprocSetFrameSize(VpsHal_Handle handle,
                             UInt32 width,
                             UInt32 height,
                             Ptr ptr);

Int32 VpsHal_cprocSetConfig(VpsHal_Handle handle,
                            Vps_CprocConfig *cprocCfg,
                            Ptr ptr);

Int32 VpsHal_cprocGetConfig(VpsHal_Handle handle,
                            Vps_CprocConfig *cprocCfg);
#ifdef __cplusplus
}
#endif

#endif /* #ifndef VPS_CPROCHAL_H */

