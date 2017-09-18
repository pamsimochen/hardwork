/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_csc.h
 *
 * \brief VPS Color Space Converter HAL header file
 * This file exposes the HAL APIs of the VPS Color Space Converter.
 *
 */

#ifndef _VPSHAL_CSC_H
#define _VPSHAL_CSC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/vps.h>


#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief CSC block on the HD1 display path which displays HD mode */
#define VPSHAL_CSC_INST_HD1             (0u)
/** \brief CSC block on SD display path */
#define VPSHAL_CSC_INST_SD              (1u)
/** \brief CSC block on the display write back path */
#define VPSHAL_CSC_INST_WB2             (2u)
/** \brief CSC block in VIP0 */
#define VPSHAL_CSC_INST_VIP0            (3u)
/** \brief CSC block in VIP1 */
#define VPSHAL_CSC_INST_VIP1            (4u)
/** \brief CSC block on the HD0 display path which displays HD mode */
#define VPSHAL_CSC_INST_HD0             (5u)
/** \brief CSC maximum number of instances */
#define VPSHAL_CSC_MAX_INST             (6u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_CscInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 *
 */
typedef struct
{
    UInt32             instId;
    /**< CSC Instance */
    UInt32             baseAddress;
    /**< Base address of the instance */
} VpsHal_CscInstParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_cscInit
 *  \brief CSC HAL init function.
 *
 *  Initializes CSC objects, gets the register overlay offsets for CSC
 *  registers.
 *  This function should be called before calling any of CSC HAL API's.
 *  VPDMA HAL should be initialized prior to calling any of CSC HAL
 *  functions.
 *  All global variables are initialized.
 *
 *  \param numInstances  Number of Noise Filter Instances and number of
 *                       array elements in the second argument
 *  \param initParams    Instance Specific Parameters
 *  \param arg           Not used currently. Meant for future purpose.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_cscInit(UInt32 numInstances,
                   const VpsHal_CscInstParams *initParams,
                   Ptr arg);

/**
 *  VpsHal_cscDeInit
 *  \brief CSC HAL exit function.
 *  Currently this function does not do anything.
 *
 *  \param arg     Not used currently. Meant for future purpose.
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_cscDeInit(Ptr arg);


/**
 *  VpsHal_cscOpen
 *  \brief Returns the handle to the requested CSC instance.
 *  This function should be called prior to calling any of the CSC HAL
 *  configuration APIs to get the instance handle.
 *  VpsHal_cscInit should be called prior to this.
 *
 *  \param cscInst  Requested CSC instance.
 *
 *  \return         Returns CSC instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_cscOpen(UInt32 cscInst);

/**
 *  VpsHal_cscClose
 *  \brief Closes the CSC HAL instance.
 *  Currently this function does not do anything. It is provided in case in the
 *  future resource management is done by individual HAL - using counters.
 *  VpsHal_cscInit and VpsHal_cscOpen should be called prior to this.
 *
 *  \param handle   Valid handle returned by VpsHal_cscOpen function.
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_cscClose(VpsHal_Handle handle);


/**
 *  VpsHal_cscGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA
 *  register configuration overlay required to program the CSC registers using
 *  VPDMA config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the CSC registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving
 *  valuable CPU in programming the VPS registers.
 *  VpsHal_cscInit and VpsHal_cscOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_cscOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
UInt32 VpsHal_cscGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  VpsHal_cscCreateConfigOvly
 *  \brief Creates the CSC register configuration
 *  overlay using VPDMA helper function.
 *
 *  This function does not initialize the overlay with CSC configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  VpsHal_cscSetConfig function before submitting the same to VPDMA for
 *  register configuration at runtime.
 *  VpsHal_cscInit and VpsHal_cscOpen should be called prior to this.
 *
 *  \param handle         Valid handle returned by VpsHal_cscOpen function.
 *
 *  \param configOvlyPtr  Pointer to the overlay memory where the overlay is
 *                        formed. The memory for the overlay should be allocated
 *                        by client driver. This parameter should be non-NULL.
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_cscCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);


/**
 *  VpsHal_cscSetConfig
 *  \brief Sets the CSC configuration to either the
 *  actual CSC registers or to the configuration overlay memory.
 *
 *  This function configures the CSC registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual CSC MMR or
 *  to configuration overlay memory.
 *  This function configures all the sub-modules in CSC using other config
 *  function call.
 *  VpsHal_cscInit and VpsHal_cscOpen should be called prior to this.
 *  CSC registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle          Valid handle returned by VpsHal_cscOpen function.
 *
 *  \param config          Pointer to Vps_CscConfig structure containing the
 *                         register configurations. This parameter should be
 *                         non-NULL.
 *
 *  \param configOvlyPtr   Pointer to the configuration overlay memory.
 *                         If this parameter is NULL, then the configuration is
 *                         written to the actual CSC registers. Otherwise the
 *                         configuration is updated in the memory pointed
 *                         by the same at proper virtual offsets. This parameter
 *                         can be NULL depending on the intended usage.
 *
 *  \return                Returns 0 on success else returns error value.
 */
Int VpsHal_cscSetConfig(VpsHal_Handle handle,
                  const Vps_CscConfig *config,
                  Ptr configOvlyPtr);

/**
 *  VpsHal_cscGetConfig
 *  \brief Gets the CSC configuration from the actual CSC registers.
 *  VpsHal_cscInit and VpsHal_cscOpen should be called prior to this.
 *
 *  \param handle  Valid handle returned by VpsHal_cscOpen function.
 *
 *  \param config  Pointer to Vps_CscConfig structure to be filled with
 *                 register configurations. This parameter should be non-NULL.
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_cscGetConfig(VpsHal_Handle handle, Vps_CscConfig *config);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_CSC_H */
