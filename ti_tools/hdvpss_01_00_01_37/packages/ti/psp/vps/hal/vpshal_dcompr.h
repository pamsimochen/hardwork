/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_dcompr.h
 *
 *  \brief VPS Decompressor HAL header file.
 *  This file exposes the HAL APIs of the VPS DECOMP module.
 *
 */

#ifndef _VPSHAL_DCOMPR_H
#define _VPSHAL_DCOMPR_H

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

/** \brief Instance for Field N-1 primary deinterlacer path. */
#define VPSHAL_DCOMPR_INST_PRI_FLD1     (0u)
/** \brief Instance for Field N-2 primary deinterlacer path. */
#define VPSHAL_DCOMPR_INST_PRI_FLD2     (1u)
/** \brief Instance for Field N-3 primary deinterlacer path. */
#define VPSHAL_DCOMPR_INST_PRI_FLD3     (2u)
/** \brief Instance for Field N-1 auxiliary deinterlacer path. */
#define VPSHAL_DCOMPR_INST_AUX_FLD1     (3u)
/** \brief Instance for Field N-2 auxiliary deinterlacer path. */
#define VPSHAL_DCOMPR_INST_AUX_FLD2     (4u)
/** \brief DCOMPR maximum number of instances */
#define VPSHAL_DCOMPR_MAX_INST          (5u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_DcomprInitParams
 *  \brief Instance specific parameters used at the init.
 */
typedef struct
{
    UInt32              instId;
    /**< Decompressor Instance. */
    UInt32              baseAddress;
    /**< Base address of the instance. */
} VpsHal_DcomprInitParams;

/**
 *  struct VpsHal_DcomprConfig
 *  \brief Configuration parameters for Decompressor.
 */
typedef struct
{
    UInt32              enable;
    /**< Flag to indicate whether decompressor to be enabled or not. */
    UInt32              width;
    /**< Width in pixels. Depending on width, other parameters like unit per
         line are calculated. */
} VpsHal_DcomprConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_dcomprInit
 *  \brief DCOMPR HAL init function.
 *
 *  Initializes DCOMPR objects, gets the register overlay offsets for DCOMPR
 *  registers.
 *  This function should be called before calling any of DCOMPR HAL API's.
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
Int32 VpsHal_dcomprInit(UInt32 numInstances,
                        const VpsHal_DcomprInitParams *initParams,
                        Ptr arg);

/**
 *  VpsHal_dcomprDeInit
 *  \brief DCOMPR HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg              For the Future use, not used currently.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_dcomprDeInit(Ptr arg);

/**
 *  VpsHal_dcomprOpen
 *  \brief Returns the handle to the requested DCOMPR instance.
 *
 *  This function should be called prior to calling any of the DCOMPR HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId           Requested DCOMPR instance.
 *
 *  \return                 Returns DCOMPR instance handle on success else
 *                          returns NULL.
 */
VpsHal_Handle VpsHal_dcomprOpen(UInt32 instId);

/**
 *  VpsHal_dcomprClose
 *  \brief Closes the DCOMPR HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle           Valid handle returned by VpsHal_dcomprOpen function.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_dcomprClose(VpsHal_Handle handle);

/**
 *  VpsHal_dcomprGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the DCOMPR registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the DCOMPR registers using VPDMA.
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
UInt32 VpsHal_dcomprGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  VpsHal_dcomprCreateConfigOvly
 *  \brief Creates the DCOMPR register configuration overlay using VPDMA
 *  helper function.
 *
 *  This function does not initialize the overlay with DCOMPR configuration.
 *  It is the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_dcomprSetConfig function before submitting the same
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
Int32 VpsHal_dcomprCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);

/**
 *  VpsHal_dcomprSetConfig
 *  \brief Sets the DCOMPR configuration to either the actual DCOMPR registers
 *  or to the configuration overlay memory.
 *
 *  This function configures the DCOMPR registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual DCOMPR MMR
 *  or to configuration overlay memory.
 *  This function configures all the sub-modules in DCOMPR using other config
 *  function call.
 *
 *  \param handle           Valid handle returned by VpsHal_dcomprOpen function.
 *  \param config           Pointer to VpsHal_DcomprConfig structure containing
 *                          the register configurations. This parameter should
 *                          be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the configuration
 *                          is written to the actual DCOMPR registers.
 *                          Otherwise the configuration is updated in the
 *                          memory pointed by the same at proper virtual
 *                          offsets. This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_dcomprSetConfig(VpsHal_Handle handle,
                             const VpsHal_DcomprConfig *config,
                             Ptr configOvlyPtr);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_DCOMPR_H */
