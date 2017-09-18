/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_drn.h
 *
 * \brief VPS De-Ringing HAL header file
 * This file exposes the HAL APIs of the VPS De-Ringer.
 *
 */

#ifndef _VPSHAL_DRN_H
#define _VPSHAL_DRN_H

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

/** \brief Instance ID for DRN on the primary path. */
#define VPSHAL_DRN_INST_PRI             (0u)
/** \brief Instance ID for DRN on the auxiliary path. */
#define VPSHAL_DRN_INST_AUX             (1u)
/** \brief CHRUS maximum number of instances */
#define VPSHAL_DRN_MAX_INST             (2u)

/**
 *  \brief enum VpsHal_DrnHorzDilateTileSize
 *  Enum for enabling horizontal Dilation for edge information.
 */
typedef enum
{
    VPSHAL_DRN_TILE_SIZE_8x8 = 0,  /* Uses 8x8 tile to control sigma filter */
    VPSHAL_DRN_TILE_SIZE_16x8      /* Uses 16x8 tile to control sigma filter */
} VpsHal_DrnHorzDilateTileSize;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief struct VpsHal_DrnInstParams
 * Structure containing instance specific parameters used at the init.
 *
 * @instId - Instance Identifier
 * @baseAddress - Physical register start address for this instance
 */
typedef struct
{
    UInt32          instId;         /* DRN Instance */
    UInt32          baseAddress;    /* Base address of the instance */
} VpsHal_DrnInstParams;

/**
 * \brief struct VpsHal_DrnConfig
 * Structure containing configuration parameters of DRN.
 *
 * @scanFormat - Scan Format i.e. Interlaced or Progressive
 * @dilateTileSize - Enables/Disables Horizontal Dilation
 * @frameWidth - Width of the Frame
 * @frameHeight - Height of the frame. Upper layer should pass correct
 *        height of the frame/field i.e. for Progressive, height
 *        is half of frame height and for interlaced, height is
 *        height of the field.
 */
typedef struct
{
    Vps_ScanFormat                  scanFormat;
    VpsHal_DrnHorzDilateTileSize    dilateTileSize;
    UInt16                          width;
    UInt16                          height;
    UInt32                          bypass;
} VpsHal_DrnConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  ======== VpsHal_vpdmaInit ========
 *  DRN HAL init function.
 *
 *  Initializes DRN objects, gets the register overlay offsets for DRN
 *  registers.
 *  This function should be called before calling any of DRN HAL API's.
 *
 *  @param(numInstance)     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  @param(initParams)      Instance Specific Parameters
 *  @param(arg)             For the Future use, not used currently.
 *
 *  @b(return)              Returns 0 on success else returns error value
 */
Int VpsHal_drnInit(UInt32 numInstances,
                   const VpsHal_DrnInstParams *initParams,
                   Ptr arg);

/**
 *  ======== VpsHal_drnDeInit ========
 *  DRN HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  @param(arg)             For the Future use, not used currently.
 *
 *  @b(return)              Returns 0 on success else returns error value
 */
Int VpsHal_drnDeInit(Ptr arg);


/**
 *  ======== VpsHal_drnOpen ========
 *  Returns the handle to the requested DRN instance.
 *
 *  This function should be called prior to calling any of the DRN HAL
 *  configuration APIs to get the instance handle.
 *
 *  @param(drnInst)         Requested DRN instance.
 *
 *  @b(return)              Handle to the DRN instance
 */
VpsHal_Handle VpsHal_drnOpen(UInt32 drnInst);

/**
 *  ======== VpsHal_drnClose ========
 *  Closes the DRN instance.
 *
 *  Currently this function does not do anything. It is provided in case in the
 *  future resource management is done by individual HAL - using counters.
 *
 *  @param(handle)          Handle to DRN
 *
 *  @b(return)              0 on success, -1 on error
 */
Int VpsHal_drnClose(VpsHal_Handle handle);


/**
 *  ======== VpsHal_drnGetConfigOvlySize ========
 *  eturns the size (in bytes) of the VPDMA
 *  register configuration overlay required to program the DRN registers using
 *  VPDMA config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the DRN registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving valuable
 *  CPU in programming the VPS registers.
 *
 *  @param(handle)          Handle to DRN
 *
 *  @b(return)              Size of the configuration register overlay
 */
UInt32 VpsHal_drnGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  ======== VpsHal_drnCreateConfigOvly ========
 *  Creates the DRN register configuration overlay using VPDMA helper function.
 *
 *  This function does not initialize the overlay with DRN configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  Drn_setConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  @param(handle)          Valid handle returned by Drn_open function.
 *  @param(configOvlyPtr)   Pointer to the overlay memory where the overlay is
 *                          formed. The memory for the overlay should be
 *                          allocated by client driver.
 *                          This parameter should be non-NULL.
 *
 *  @b(return)              Returns 0 on success else returns error value.
 */
Int VpsHal_drnCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);


/**
 *  ======== VpsHal_drnSetConfig ========
 *  Sets the DRN configuration to either the
 *  actual DRN registers or to the configuration overlay memory.
 *
 *  This function configures the DRN registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual DRN MMR or
 *  to configuration overlay memory.
 *  This function configures all the sub-modules in DRN using other config
 *  function call.
 *
 *  @param(handle)          Valid handle returned by Drn_open function.
 *  @param(config)          Pointer to VpsHal_DrnConfig structure containing
 *                          the register configurations. This parameter
 *                          should be non-NULL.
 *  @param(configOvlyPtr)   Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the configuration
 *                          is written to the actual DRN registers. Otherwise
 *                          the configuration is updated in the memory pointed
 *                          by the same at proper virtual offsets. This
 *                          parameter can be NULL depending
 *                          on the intended usage.
 *
 *  @b(return)              Returns 0 on success else returns error value.
 */
Int VpsHal_drnSetConfig(VpsHal_Handle handle,
                        const VpsHal_DrnConfig *config,
                        Ptr configOvlyPtr);

/**
 *  ======== VpsHal_drnGetConfig ========
 *  Gets the DRN configuration from the actual DRN registers.
 *
 *  @param(handle)          Valid handle returned by Drn_open function.
 *  @param(config)          Pointer to VpsHal_DrnConfig structure to be filled
 *                          with register configurations. This parameter
 *                          should be non-NULL.
 *
 *  @b(return)              Returns 0 on success else returns error value.
 */
Int VpsHal_drnGetConfig(VpsHal_Handle handle, VpsHal_DrnConfig *config);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_DRN_H */
