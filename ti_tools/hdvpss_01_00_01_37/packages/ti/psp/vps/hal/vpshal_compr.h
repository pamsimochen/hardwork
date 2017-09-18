/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_compr.h
 *
 *  \brief VPS Compressor HAL header file.
 *  This file exposes the HAL APIs of the VPS COMPR module.
 *
 */

#ifndef _VPSHAL_COMPR_H
#define _VPSHAL_COMPR_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Instance for Top field in primary deinterlacer path. */
#define VPSHAL_COMPR_INST_PRI_TOP       (0u)
/** \brief Instance for Bottom field in primary deinterlacer path. */
#define VPSHAL_COMPR_INST_PRI_BOT       (1u)
/** \brief Instance for auxiliary deinterlacer path. */
#define VPSHAL_COMPR_INST_AUX           (2u)
/** \brief Compressor maximum number of instances */
#define VPSHAL_COMPR_MAX_INST           (3u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_ComprInitParams
 *  \brief Instance specific parameters used at the init.
 */
typedef struct
{
    UInt32              instId;
    /**< Compressor Instance. */
    UInt32              baseAddress;
    /**< Base address of the instance. */
} VpsHal_ComprInitParams;

/**
 *  struct VpsHal_ComprConfig
 *  \brief Configuration parameters for Compressor.
 */
typedef struct
{
    UInt32              enable;
    /**< Flag to indicate whether compressor to be enabled or not. */
    UInt32              width;
    /**< Width in pixels. Depending on width, other parameters like unit size
         unit bit size are calculated. */
} VpsHal_ComprConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_comprInit
 *  \brief COMPR HAL init function.
 *
 *  Initializes COMPR objects, gets the register overlay offsets for COMPR
 *  registers.
 *  This function should be called before calling any of COMPR HAL API's.
 *
 *  \param numInstances     Number of instance objects to be initialized
 *  \param initParams       Pointer to the instance parameter containing
 *                          instance specific information. If the number of
 *                          instance is greater than 1, then this pointer
 *                          should point to an array of init parameter
 *                          structure of size numInstances.
 *                          This parameter should not be NULL.
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_comprInit(UInt32 numInstances,
                       const VpsHal_ComprInitParams *initParams,
                       Ptr arg);

/**
 *  VpsHal_comprDeInit
 *  \brief COMPR HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg              For the Future use, not used currently.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_comprDeInit(Ptr arg);

/**
 *  VpsHal_comprOpen
 *  \brief Returns the handle to the requested COMPR instance.
 *
 *  This function should be called prior to calling any of the COMPR HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId           Requested COMPR instance.
 *
 *  \return                 Returns COMPR instance handle on success else
 *                          returns NULL.
 */
VpsHal_Handle VpsHal_comprOpen(UInt32 instId);

/**
 *  VpsHal_comprClose
 *  \brief Closes the COMPR HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle           Valid handle returned by VpsHal_comprOpen function.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_comprClose(VpsHal_Handle handle);

/**
 *  VpsHal_comprGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the COMPR registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the COMPR registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context
 *  (multiple instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay
 *  one-time and submit this to VPDMA everytime when context changes.
 *  Thus saving valuable CPU in programming the VPS registers.
 *
 *  \param handle           Instance handle
 *
 *  \return                 Returns the size of config overlay memory
 */
UInt32 VpsHal_comprGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  VpsHal_comprCreateConfigOvly
 *  \brief Creates the COMPR register configuration overlay using VPDMA
 *  helper function.
 *
 *  This function does not initialize the overlay with COMPR configuration.
 *  It is the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_comprSetConfig function before submitting the same
 *  to VPDMA for register configuration at runtime.
 *
 *  \param handle           Instance handle
 *  \param configOvlyPtr    Pointer to the overlay memory where the
 *                          overlay is formed. The memory for the overlay
 *                          should be allocated by client driver.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_comprCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);

/**
 *  VpsHal_comprSetConfig
 *  \brief Sets the COMPR configuration to either the actual COMPR registers
 *  or to the configuration overlay memory.
 *
 *  This function configures the COMPR registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual COMPR MMR
 *  or to configuration overlay memory.
 *  This function configures all the sub-modules in COMPR using other config
 *  function call.
 *
 *  \param handle           Valid handle returned by VpsHal_comprOpen function.
 *  \param config           Pointer to VpsHal_ComprConfig structure containing
 *                          the register configurations. This parameter should
 *                          be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the configuration
 *                          is written to the actual COMPR registers.
 *                          Otherwise the configuration is updated in the
 *                          memory pointed by the same at proper virtual
 *                          offsets. This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_comprSetConfig(VpsHal_Handle handle,
                            const VpsHal_ComprConfig *config,
                            Ptr configOvlyPtr);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_COMPR_H */
