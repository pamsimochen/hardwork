/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_dei.c
 *
 *  \brief VPS Deinterlacer HAL file.
 *  This file implements the HAL APIs of the VPS Deinterlacer.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/cslr/cslr_vps_dei.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_dei.h>
#include <ti/psp/vps/hal/src/vpshalDeiDefaults.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum VpsHal_DeiConfigOvlyIdx
 *  \brief Enums for the configuration overlay register offsets - used as array
 *  indices to register offset array. These enums defines the registers
 *  that will be used to form the configuration register overlay.
 *  This in turn is used by client drivers to configure registers using
 *  VPDMA during context-switching (software channels with different
 *  configurations).
 *
 *  Note: If in future it is decided to remove or add some register, this
 *  enum should be modified accordingly.
 *  And make sure that the values of these enums are in ascending order
 *  and it starts from 0.
 */
typedef enum
{
    VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG0 = 0,
    VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG1,
    VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG3,
    VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG8,
    VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG9,
    VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG10,
    VPSHAL_DEI_NUM_REG_IDX              /* This should be the last enum */
} VpsHal_DeiConfigOvlyIdx;

/**
 *  Macros for maximum value possible for some of the bit fields used.
 */
#define VPSHAL_DEI_MAX_DEI_WIDTH                                               \
                    (CSL_VPS_DEI_DEI_REG0_WIDTH_MASK >>                        \
                     CSL_VPS_DEI_DEI_REG0_WIDTH_SHIFT)
#define VPSHAL_DEI_MAX_DEI_HEIGHT                                              \
                    (CSL_VPS_DEI_DEI_REG0_HEIGHT_MASK >>                       \
                     CSL_VPS_DEI_DEI_REG0_HEIGHT_SHIFT)
#define VPSHAL_DEI_MAX_FMD_WINDOW_MINX                                         \
                    (CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MINX_MASK >>              \
                     CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MINX_SHIFT)
#define VPSHAL_DEI_MAX_FMD_WINDOW_MINY                                         \
                    (CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MINY_MASK >>              \
                     CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MINY_SHIFT)
#define VPSHAL_DEI_MAX_FMD_WINDOW_MAXX                                         \
                    (CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MAXX_MASK >>              \
                     CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MAXX_SHIFT)
#define VPSHAL_DEI_MAX_FMD_WINDOW_MAXY                                         \
                    (CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MAXY_MASK >>              \
                     CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MAXY_SHIFT)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_DeiObj
 *  \brief Per instance information about each DEI module.
 */
typedef struct
{
    UInt32              instId;
    /**< Instance number/ID */
    UInt32              openCnt;
    /**< Counter to keep track of number of open calls for an instance */
    CSL_VpsDeiRegsOvly  baseAddress;
    /**< Register overlay pointer */
    UInt32              configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay memory
         required */
    UInt32             *regOffset[VPSHAL_DEI_NUM_REG_IDX];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA */
    UInt32              virRegOffset[VPSHAL_DEI_NUM_REG_IDX];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to regOffset member */
} VpsHal_DeiObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 halDeiInitInstance(VpsHal_DeiObj *deiObj);

static Int32 halDeiSetExpertConfig(VpsHal_DeiObj *deiObj,
                                   const VpsHal_DeiExpertConfig *config);

static inline UInt32 halDeiMakeDeiReg0Reg(UInt32 initRegVal,
                                          UInt16 width,
                                          UInt16 height,
                                          UInt32 interlaceBypass,
                                          UInt32 fieldFlush,
                                          UInt32 progressiveBypass);
static inline UInt32 halDeiMakeDeiReg1Reg(UInt32 initRegVal,
                                          UInt32 spatMaxBypass,
                                          UInt32 tempMaxBypass);
static inline UInt32 halDeiMakeDeiReg3Reg(UInt32 initRegVal,
                                          UInt32 inpMode,
                                          UInt32 tempInpEnable,
                                          UInt32 tempInpChromaEnable);
static inline UInt32 halDeiMakeDeiReg8Reg(UInt32 initRegVal,
                                          UInt16 windowMinx,
                                          UInt16 windowMaxx,
                                          UInt32 window);
static inline UInt32 halDeiMakeDeiReg9Reg(UInt32 initRegVal,
                                          UInt16 windowMiny,
                                          UInt16 windowMaxy);
static inline UInt32 halDeiMakeDeiReg10Reg(UInt32 initRegVal,
                                           UInt32 filmMode,
                                           UInt32 bed,
                                           UInt32 lock,
                                           UInt32 jamDir);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  DEI objects - Module variable to store information about each DEI instance.
 *  Note: If the number of DEI instance increase, then VPSHAL_DEI_MAX_INST
 *  macro should be changed accordingly.
 */
static VpsHal_DeiObj DeiObjects[VPSHAL_DEI_MAX_INST];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_deiInit
 *  \brief DEI HAL init function.
 *
 *  Initializes DEI objects, gets the register overlay offsets for DEI
 *  registers.
 *  This function should be called before calling any of DEI HAL API's.
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
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int VpsHal_deiInit(UInt32 numInstances,
                   const VpsHal_DeiInitParams *initParams,
                   Ptr arg)
{
    Int             instCnt;
    VpsHal_DeiObj  *deiObj;
    Int             retVal = VPS_SOK;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_DEI_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    /* Initialize DEI Objects to zero */
    VpsUtils_memset(DeiObjects, 0, sizeof(DeiObjects));

    for (instCnt = 0; instCnt < numInstances; instCnt++)
    {
        GT_assert(VpsHalTrace,
                    (initParams[instCnt].instId < VPSHAL_DEI_MAX_INST));

        deiObj = &DeiObjects[instCnt];
        deiObj->instId = initParams[instCnt].instId;
        deiObj->baseAddress =
            (CSL_VpsDeiRegsOvly) initParams[instCnt].baseAddress;

        /* Initialize the instance */
        retVal += halDeiInitInstance(deiObj);
    }

    return (retVal);
}



/**
 *  VpsHal_deiDeInit
 *  \brief DEI HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int VpsHal_deiDeInit(Ptr arg)
{
    return (VPS_SOK);
}



/**
 *  VpsHal_deiOpen
 *  \brief Returns the handle to the requested DEI instance.
 *
 *  This function should be called prior to calling any of the DEI HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId           Requested DEI instance.
 *
 *  \return                 Returns DEI instance handle on success else
 *                          returns NULL.
 */
VpsHal_Handle VpsHal_deiOpen(UInt32 instId)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;

    /* Check if instance number is valid */
    GT_assert(VpsHalTrace, (instId < VPSHAL_DEI_MAX_INST));

    for (cnt = 0; cnt < VPSHAL_DEI_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (instId == DeiObjects[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0u == DeiObjects[cnt].openCnt)
            {
                handle = (VpsHal_Handle) &DeiObjects[cnt];
                DeiObjects[cnt].openCnt++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }

    return (handle);
}



/**
 *  VpsHal_deiClose
 *  \brief Closes the DEI HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle           Valid handle returned by VpsHal_deiOpen function
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int VpsHal_deiClose(VpsHal_Handle handle)
{
    Int             retVal = VPS_EFAIL;
    UInt32          cookie;
    VpsHal_DeiObj  *deiObj;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    deiObj = (VpsHal_DeiObj *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (deiObj->openCnt > 0u)
    {
        deiObj->openCnt--;
        retVal = VPS_SOK;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  VpsHal_deiGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the DEI registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the DEI registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context
 *  (multiple instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay
 *  one-time and submit this to VPDMA everytime when context changes.
 *  Thus saving valuable CPU in programming the VPS registers.
 *
 *  \param handle           Instance handle
 *  \return                 Returns the size of config overlay memory
 */
UInt32 VpsHal_deiGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32          configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((VpsHal_DeiObj *) handle)->configOvlySize;

    return (configOvlySize);
}



/**
 *  VpsHal_deiCreateConfigOvly
 *  \brief Creates the DEI register configuration overlay using VPDMA
 *  helper function.
 *
 *  This function does not initialize the overlay with DEI configuration.
 *  It is the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_deiSetConfig function before submitting the same
 *  to VPDMA for register configuration at runtime.
 *
 *  \param handle           Instance handle
 *  \param configOvlyPtr    Pointer to the overlay memory where the
 *                          overlay is formed. The memory for the overlay
 *                          should be allocated by client driver.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int32           retVal;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    retVal = VpsHal_vpdmaCreateRegOverlay(
                 ((VpsHal_DeiObj *) handle)->regOffset,
                 VPSHAL_DEI_NUM_REG_IDX,
                 configOvlyPtr);

    return (retVal);
}



/**
 *  VpsHal_deiSetConfig
 *  \brief Sets the entire DEI configuration to either the actual DEI
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEI registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEI MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEI using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEI registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiSetConfig(VpsHal_Handle handle,
                          const VpsHal_DeiConfig *config,
                          Ptr configOvlyPtr)
{
    Int32               retVal;
    UInt32              height;
    VpsHal_DeiObj      *deiObj;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;
    UInt32             *virRegOffset;
    UInt32              interlaceBypass;
    UInt32              progressiveBypass;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->width <= VPSHAL_DEI_MAX_DEI_WIDTH));
    GT_assert(VpsHalTrace, (config->height <= VPSHAL_DEI_MAX_DEI_HEIGHT));

    /* Depending on mode and bypass parameters, decide the bypass values */
    if (VPS_SF_INTERLACED == config->scanFormat)
    {
        if (TRUE == config->bypass)
        {
            /* For the interlaced bypass mode, output height is field height */
            height = config->height;
            interlaceBypass = TRUE;
            progressiveBypass = FALSE;
        }
        else /* Bypass disabled - Normal operation */
        {
            /* For the de-interlacing mode, output height is frame height */
            height = config->height * 2u;
            interlaceBypass = FALSE;
            progressiveBypass = FALSE;
        }
    }
    else /* Progressive mode */
    {
        /* For interlaced, config parameters has frame height. For
           progressive bypass mode, DEI should
           be configured with the half of the frame size */
        height = config->height;
        /* For progressive inputs, irrespective of the bypass flag DEI should
         * be set in bypass mode only */
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        /* Progressive bypass mode is not supported in TI814x DEI, so make it
         * interlaced bypass */
        interlaceBypass = TRUE;
        progressiveBypass = FALSE;
#else
        interlaceBypass = FALSE;
        progressiveBypass = TRUE;
#endif
    }

    deiObj = (VpsHal_DeiObj *) handle;
    regOvly = deiObj->baseAddress;

    /* Make register */
    tempReg = halDeiMakeDeiReg0Reg(
                  regOvly->DEI_REG0,
                  config->width,
                  height,
                  interlaceBypass,
                  config->fieldFlush,
                  progressiveBypass);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deiObj->virRegOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG0];
        *virRegOffset = tempReg;
    }

    /* Call individual functions to set the configuration */
    retVal = VpsHal_deiSetMdtConfig(handle, &config->mdt, configOvlyPtr);
    retVal += VpsHal_deiSetEdiConfig(handle, &config->edi, configOvlyPtr);
    retVal += VpsHal_deiSetFmdConfig(
                  handle,
                  &config->fmd,
                  configOvlyPtr,
                  config->width,
                  config->height);

    return (retVal);
}



/**
 *  VpsHal_deiSetFrameSize
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEI registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deiSetFrameSize(VpsHal_Handle handle,
                             const VpsHal_DeiConfig *config,
                             Ptr configOvlyPtr)
{
    Int32               retVal = VPS_SOK;
    UInt32              height;
    VpsHal_DeiObj      *deiObj;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;
    UInt32              interlaceBypass;
    UInt32              progressiveBypass;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->width <= VPSHAL_DEI_MAX_DEI_WIDTH));
    GT_assert(VpsHalTrace, (config->height <= VPSHAL_DEI_MAX_DEI_HEIGHT));

    /* Depending on mode and bypass parameters, decide the bypass values */
    if (VPS_SF_INTERLACED == config->scanFormat)
    {
        if (TRUE == config->bypass)
        {
            /* For the interlaced bypass mode, output height is field height */
            height = config->height;
            interlaceBypass = TRUE;
            progressiveBypass = FALSE;
        }
        else /* Bypass disabled - Normal operation */
        {
            /* For the de-interlacing mode, output height is frame height */
            height = config->height * 2u;
            interlaceBypass = FALSE;
            progressiveBypass = FALSE;
        }
    }
    else /* Progressive mode */
    {
        /* For interlaced, config parameters has frame height. For
           progressive bypass mode, DEI should
           be configured with the half of the frame size */
        height = config->height;
        /* For progressive inputs, irrespective of the bypass flag DEI should
         * be set in bypass mode only */
#if defined(TI_814X_BUILD) || defined(TI_813X_BUILD) || defined(TI_811X_BUILD) || defined(TI_8149_BUILD)
        /* Progressive bypass mode is not supported in TI814x DEI, so make it
         * interlaced bypass */
        interlaceBypass = TRUE;
        progressiveBypass = FALSE;
#else
        interlaceBypass = FALSE;
        progressiveBypass = TRUE;
#endif
    }

    deiObj = (VpsHal_DeiObj *) handle;
    regOvly = deiObj->baseAddress;

    /* Make register */
    tempReg = halDeiMakeDeiReg0Reg(
                  regOvly->DEI_REG0,
                  config->width,
                  height,
                  interlaceBypass,
                  config->fieldFlush,
                  progressiveBypass);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* No Support for Writing Size through Configuration overlay */
    }

    return (retVal);
}



/**
 *  VpsHal_deiSetMdtConfig
 *  \brief Sets the entire DEI MDT configuration to either the actual DEI
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEI MDT registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEI MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEI using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiMdtConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEI registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiSetMdtConfig(VpsHal_Handle handle,
                             const VpsHal_DeiMdtConfig *config,
                             Ptr configOvlyPtr)
{
    VpsHal_DeiObj      *deiObj;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    deiObj = (VpsHal_DeiObj *) handle;
    regOvly = deiObj->baseAddress;

    /* Make register */
    tempReg = halDeiMakeDeiReg1Reg(
                  regOvly->DEI_REG1,
                  config->spatMaxBypass,
                  config->tempMaxBypass);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG1 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deiObj->virRegOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG1];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deiSetEdiConfig
 *  \brief Sets the entire DEI EDI configuration to either the actual DEI
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEI EDI registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEI MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEI using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiEdiConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEI registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiSetEdiConfig(VpsHal_Handle handle,
                             const VpsHal_DeiEdiConfig *config,
                             Ptr configOvlyPtr)
{
    VpsHal_DeiObj      *deiObj;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));
    /* If temporal Interpolation for luma is disabled, then temporal
     * interpolation for chroma should also be disabled */
    GT_assert(VpsHalTrace, (!((FALSE == config->tempInpEnable) &&
                            (TRUE == config->tempInpChromaEnable))));

    deiObj = (VpsHal_DeiObj *) handle;
    regOvly = deiObj->baseAddress;

    /* Make register */
    tempReg = halDeiMakeDeiReg3Reg(
                  regOvly->DEI_REG3,
                  config->inpMode,
                  config->tempInpEnable,
                  config->tempInpChromaEnable);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG3 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deiObj->virRegOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG3];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deiSetFmdConfig
 *  \brief Sets the entire DEI FMD configuration to either the actual DEI
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEI FMD registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEI MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEI using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiFmdConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEI registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *  \param width            Picture width at output.
 *  \param height           Picture height at output.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiSetFmdConfig(VpsHal_Handle handle,
                             const VpsHal_DeiFmdConfig *config,
                             Ptr configOvlyPtr,
                             UInt32 width,
                             UInt32 height)
{
    VpsHal_DeiObj      *deiObj;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;
    UInt32             *virRegOffset;
    UInt32              windowWidth, windowHeight;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace,
        (config->windowMinx <= VPSHAL_DEI_MAX_FMD_WINDOW_MINX));
    GT_assert(VpsHalTrace,
        (config->windowMiny <= VPSHAL_DEI_MAX_FMD_WINDOW_MINY));
    GT_assert(VpsHalTrace,
        (config->windowMaxx <= VPSHAL_DEI_MAX_FMD_WINDOW_MAXX));
    GT_assert(VpsHalTrace,
        (config->windowMaxy <= VPSHAL_DEI_MAX_FMD_WINDOW_MAXY));

    deiObj = (VpsHal_DeiObj *) handle;
    regOvly = deiObj->baseAddress;

    /*
     * Check whether the window size is less than the already set values
     */
    windowWidth = (config->windowMaxx - config->windowMinx) + 1u;
    windowHeight = (config->windowMaxy - config->windowMiny) + 1u;
    GT_assert(VpsHalTrace, (windowWidth <= width));
    GT_assert(VpsHalTrace, (windowHeight <= height));

    /* Make register */
    tempReg = halDeiMakeDeiReg8Reg(
                  regOvly->DEI_REG8,
                  config->windowMinx,
                  config->windowMaxx,
                  config->window);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG8 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deiObj->virRegOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG8];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halDeiMakeDeiReg9Reg(
                  regOvly->DEI_REG9,
                  config->windowMiny,
                  config->windowMaxy);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG9 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deiObj->virRegOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG9];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halDeiMakeDeiReg10Reg(
                  regOvly->DEI_REG10,
                  config->filmMode,
                  config->bed,
                  config->lock,
                  config->jamDir);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_REG10 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deiObj->virRegOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG10];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deiGetConfig
 *  \brief Gets the entire DEI configuration from the actual DEI registers.
 *
 *  This function gets the entire DEI configuration. This function
 *  gets the configuration of all the sub-modules in DEI using other
 *  get configuration function calls.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiGetConfig(VpsHal_Handle handle, VpsHal_DeiConfig *config)
{
    Int32               retVal = VPS_EFAIL;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;
    UInt32              interlaceBypass;
    UInt32              progressiveBypass;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Get DEI configurations */
    tempReg = regOvly->DEI_REG0;
    config->width = ((tempReg & CSL_VPS_DEI_DEI_REG0_WIDTH_MASK) >>
                          CSL_VPS_DEI_DEI_REG0_WIDTH_SHIFT);
    config->height = ((tempReg & CSL_VPS_DEI_DEI_REG0_HEIGHT_MASK) >>
                          CSL_VPS_DEI_DEI_REG0_HEIGHT_SHIFT);

    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG0_FIELD_FLUSH_MASK))
    {
        config->fieldFlush = TRUE;
    }
    else            /* Field flush disabled */
    {
        config->fieldFlush = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG0_PROGRESSIVE_BYPASS_MASK))
    {
        progressiveBypass = TRUE;
    }
    else            /* Progressive bypass disabled */
    {
        progressiveBypass = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG0_INTERLACE_BYPASS_MASK))
    {
        interlaceBypass = TRUE;
    }
    else            /* Interlace bypass disabled */
    {
        interlaceBypass = FALSE;
    }

    if (TRUE == interlaceBypass)
    {
        if (TRUE == progressiveBypass)
        {
            /* Invalid state - This can never happen */
            GT_0trace(VpsHalTrace, GT_ERR,
                "VpsHal_deiGetConfig: Invalid bypass settings!!\n");
            config->scanFormat = VPS_SF_PROGRESSIVE;
            config->bypass = TRUE;
        }
        else        /* Progressive Bypass Disabled */
        {
            /* Deinterlace bypass operation */
            config->scanFormat = VPS_SF_INTERLACED;
            config->bypass = TRUE;
        }
    }
    else            /* Interlace Bypass Disabled */
    {
        if (TRUE == progressiveBypass)
        {
            /* Progressive bypass operation */
            config->scanFormat = VPS_SF_PROGRESSIVE;
            config->bypass = TRUE;
        }
        else        /* Progressive Bypass Disabled */
        {
            /* Normal deinterlace operation */
            config->scanFormat = VPS_SF_INTERLACED;
            config->bypass = FALSE;
        }
    }

    /* Call individual functions to get the configuration */
    retVal = VpsHal_deiGetMdtConfig(handle, &config->mdt);
    retVal += VpsHal_deiGetEdiConfig(handle, &config->edi);
    retVal += VpsHal_deiGetFmdConfig(handle, &config->fmd);

    return (retVal);
}



/**
 *  VpsHal_deiGetMdtConfig
 *  \brief Gets DEI MDT configuration from the actual DEI registers.
 *
 *  This function gets the DEI configuration specific to MDT module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiMdtConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiGetMdtConfig(VpsHal_Handle handle, VpsHal_DeiMdtConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Get MDT configurations */
    tempReg = regOvly->DEI_REG1;
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG1_MDT_SPATMAX_BYPASS_MASK))
    {
        config->spatMaxBypass = TRUE;
    }
    else            /* Spatial Max filter enabled */
    {
        config->spatMaxBypass = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG1_MDT_TEMPMAX_BYPASS_MASK))
    {
        config->tempMaxBypass = TRUE;
    }
    else            /* Temporal Max filter enabled */
    {
        config->tempMaxBypass = FALSE;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deiGetEdiConfig
 *  \brief Gets DEI EDI configuration from the actual DEI registers.
 *
 *  This function gets the DEI configuration specific to EDI module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiEdiConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiGetEdiConfig(VpsHal_Handle handle, VpsHal_DeiEdiConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Get EDI configurations */
    tempReg = regOvly->DEI_REG3;
    config->inpMode = (UInt32) ((tempReg &
                          CSL_VPS_DEI_DEI_REG3_EDI_INP_MODE_MASK) >>
                          CSL_VPS_DEI_DEI_REG3_EDI_INP_MODE_SHIFT);
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG3_EDI_ENABLE_3D_MASK))
    {
        config->tempInpEnable = TRUE;
    }
    else            /* Temporal interpolation disabled */
    {
        config->tempInpEnable = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG3_EDI_CHROMA_3D_ENABLE_MASK))
    {
        config->tempInpChromaEnable = TRUE;
    }
    else            /* Temporal interpolation for chroma disabled */
    {
        config->tempInpChromaEnable = FALSE;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deiGetFmdConfig
 *  \brief Gets DEI FMD configuration from the actual DEI registers.
 *
 *  This function gets the DEI configuration specific to FMD module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiFmdConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiGetFmdConfig(VpsHal_Handle handle, VpsHal_DeiFmdConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Get FMD configurations from DEI_REG8 */
    tempReg = regOvly->DEI_REG8;
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_ENABLE_MASK))
    {
        config->window = TRUE;
    }
    else            /* FMD window disabled */
    {
        config->window = FALSE;
    }
    config->windowMinx = ((tempReg &
                             CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MINX_MASK) >>
                             CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MINX_SHIFT);
    config->windowMaxx = ((tempReg &
                             CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MAXX_MASK) >>
                             CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MAXX_SHIFT);

    /* Get FMD configurations from DEI_REG9 */
    tempReg = regOvly->DEI_REG9;
    config->windowMiny = ((tempReg &
                             CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MINY_MASK) >>
                             CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MINY_SHIFT);
    config->windowMaxy = ((tempReg &
                             CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MAXY_MASK) >>
                             CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MAXY_SHIFT);

    /* Get FMD configurations from DEI_REG10 */
    tempReg = regOvly->DEI_REG10;
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG10_FMD_ENABLE_MASK))
    {
        config->filmMode = TRUE;
    }
    else            /* Film mode detection disabled */
    {
        config->filmMode = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG10_FMD_BED_ENABLE_MASK))
    {
        config->bed = TRUE;
    }
    else            /* Bad edit detection disabled */
    {
        config->bed = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG10_FMD_LOCK_MASK))
    {
        config->lock = TRUE;
    }
    else            /* FMD Lock disabled */
    {
        config->lock = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG10_FMD_JAM_DIR_MASK))
    {
        config->jamDir = VPS_DEI_FMDJAMDIR_NEXT_FLD;
    }
    else            /* Jam direction - Previous field */
    {
        config->jamDir = VPS_DEI_FMDJAMDIR_PREV_FLD;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deiGetFmdStatus
 *  \brief Returns DEI FMD status information that is needed by the FMD
 *  software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param status           Pointer to VpsHal_DeiFmdStatus structure
 *                          containing the register status.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiGetFmdStatus(VpsHal_Handle handle, VpsHal_DeiFmdStatus *status)
{
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != status));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Get FMD CAF and Reset status */
    tempReg = regOvly->DEI_REG12;
    status->caf = ((tempReg & CSL_VPS_DEI_DEI_REG12_FMD_CAF_MASK) >>
                      CSL_VPS_DEI_DEI_REG12_FMD_CAF_SHIFT);
    if (0 != (tempReg & CSL_VPS_DEI_DEI_REG12_FMD_RESET_MASK))
    {
        status->reset = TRUE;
    }
    else            /* FMD reset disabled */
    {
        status->reset = FALSE;
    }

    /* Get FMD Field Difference status */
    tempReg = regOvly->DEI_REG13;
    status->fldDiff = ((tempReg &
                          CSL_VPS_DEI_DEI_REG13_FMD_FIELD_DIFF_MASK) >>
                          CSL_VPS_DEI_DEI_REG13_FMD_FIELD_DIFF_SHIFT);

    /* Get FMD Frame Difference status */
    tempReg = regOvly->DEI_REG14;
    status->frameDiff = ((tempReg &
                            CSL_VPS_DEI_DEI_REG14_FMD_FRAME_DIFF_MASK) >>
                            CSL_VPS_DEI_DEI_REG14_FMD_FRAME_DIFF_SHIFT);

    return (VPS_SOK);
}



/**
 *  VpsHal_deiUpdateFmdConfig
 *  \brief Updates DEI FMD configuration that needs to be updated by the
 *  FMD software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeiFmdUpdateConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deiUpdateFmdConfig(VpsHal_Handle handle,
                                const VpsHal_DeiFmdUpdateConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Reset the bit fields which are being set */
    tempReg = regOvly->DEI_REG10;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG10_FMD_LOCK_MASK |
                 CSL_VPS_DEI_DEI_REG10_FMD_JAM_DIR_MASK);

    /* Set lock to film mode field */
    if (TRUE == config->lock)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG10_FMD_LOCK_MASK;
    }
    if (VPS_DEI_FMDJAMDIR_NEXT_FLD == config->jamDir)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG10_FMD_JAM_DIR_MASK;
    }

    /* Write to actual register */
    regOvly->DEI_REG10 = tempReg;

    return (VPS_SOK);
}



/**
 *  VpsHal_deiSetAdvConfig
 */
Int32 VpsHal_deiSetAdvConfig(VpsHal_Handle handle,
                             const Vps_DeiRdWrAdvCfg *advCfg,
                             Ptr configOvlyPtr)
{
    VpsHal_DeiObj      *deiObj;
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != advCfg));

    deiObj = (VpsHal_DeiObj *) handle;
    regOvly = deiObj->baseAddress;

    /* Write MDT configurations to DEI_REG2 */
    tempReg = regOvly->DEI_REG2;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG2_MDT_MVSTMAX_COR_THR_MASK |
                 CSL_VPS_DEI_DEI_REG2_MDT_MV_COR_THR_MASK |
                 CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR1_MASK |
                 CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR2_MASK |
                 CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR3_MASK);
    tempReg |= (advCfg->mdtCfg.mvstmMaxCorThr <<
        CSL_VPS_DEI_DEI_REG2_MDT_MVSTMAX_COR_THR_SHIFT);
    tempReg |= (advCfg->mdtCfg.mvCorThr <<
        CSL_VPS_DEI_DEI_REG2_MDT_MV_COR_THR_SHIFT);
    tempReg |= (advCfg->mdtCfg.sfScThr[0u] <<
        CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR1_SHIFT);
    tempReg |= (advCfg->mdtCfg.sfScThr[1u] <<
        CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR2_SHIFT);
    tempReg |= (advCfg->mdtCfg.sfScThr[2u] <<
        CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR3_SHIFT);
    regOvly->DEI_REG2 = tempReg;

    /* Write MDT configurations to DEI_REG3 */
    tempReg = regOvly->DEI_REG3;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG3_EDI_COR_SCALE_FACTOR_MASK |
                 CSL_VPS_DEI_DEI_REG3_EDI_DIR_COR_LOWER_THR_MASK |
                 CSL_VPS_DEI_DEI_REG3_EDI_CHROMA3D_COR_THR_MASK);
    tempReg |= (advCfg->ediCfg.corScaleFactor <<
        CSL_VPS_DEI_DEI_REG3_EDI_COR_SCALE_FACTOR_SHIFT);
    tempReg |= (advCfg->ediCfg.detCorLowerThr <<
        CSL_VPS_DEI_DEI_REG3_EDI_DIR_COR_LOWER_THR_SHIFT);
    tempReg |= (advCfg->ediCfg.chroma3DCorThr <<
        CSL_VPS_DEI_DEI_REG3_EDI_CHROMA3D_COR_THR_SHIFT);
    regOvly->DEI_REG3 = tempReg;

    /* Write EDI LUT configurations to DEI_REG4 */
    tempReg = regOvly->DEI_REG4;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG4_EDI_LUT0_MASK |
                 CSL_VPS_DEI_DEI_REG4_EDI_LUT1_MASK |
                 CSL_VPS_DEI_DEI_REG4_EDI_LUT2_MASK |
                 CSL_VPS_DEI_DEI_REG4_EDI_LUT3_MASK);
    tempReg |= (advCfg->ediCfg.lut[0] << CSL_VPS_DEI_DEI_REG4_EDI_LUT0_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[1] << CSL_VPS_DEI_DEI_REG4_EDI_LUT1_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[2] << CSL_VPS_DEI_DEI_REG4_EDI_LUT2_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[3] << CSL_VPS_DEI_DEI_REG4_EDI_LUT3_SHIFT);
    regOvly->DEI_REG4 = tempReg;

    /* Write EDI LUT configurations to DEI_REG5 */
    tempReg = regOvly->DEI_REG5;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG5_EDI_LUT4_MASK |
                 CSL_VPS_DEI_DEI_REG5_EDI_LUT5_MASK |
                 CSL_VPS_DEI_DEI_REG5_EDI_LUT6_MASK |
                 CSL_VPS_DEI_DEI_REG5_EDI_LUT7_MASK);
    tempReg |= (advCfg->ediCfg.lut[4] << CSL_VPS_DEI_DEI_REG5_EDI_LUT4_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[5] << CSL_VPS_DEI_DEI_REG5_EDI_LUT5_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[6] << CSL_VPS_DEI_DEI_REG5_EDI_LUT6_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[7] << CSL_VPS_DEI_DEI_REG5_EDI_LUT7_SHIFT);
    regOvly->DEI_REG5 = tempReg;

    /* Write EDI LUT configurations to DEI_REG6 */
    tempReg = regOvly->DEI_REG6;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG6_EDI_LUT8_MASK |
                 CSL_VPS_DEI_DEI_REG6_EDI_LUT9_MASK |
                 CSL_VPS_DEI_DEI_REG6_EDI_LUT10_MASK |
                 CSL_VPS_DEI_DEI_REG6_EDI_LUT11_MASK);
    tempReg |= (advCfg->ediCfg.lut[8] << CSL_VPS_DEI_DEI_REG6_EDI_LUT8_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[9] << CSL_VPS_DEI_DEI_REG6_EDI_LUT9_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[10] << CSL_VPS_DEI_DEI_REG6_EDI_LUT10_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[11] << CSL_VPS_DEI_DEI_REG6_EDI_LUT11_SHIFT);
    regOvly->DEI_REG6 = tempReg;

    /* Write EDI LUT configurations to DEI_REG7 */
    tempReg = regOvly->DEI_REG7;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG7_EDI_LUT12_MASK |
                 CSL_VPS_DEI_DEI_REG7_EDI_LUT13_MASK |
                 CSL_VPS_DEI_DEI_REG7_EDI_LUT14_MASK |
                 CSL_VPS_DEI_DEI_REG7_EDI_LUT15_MASK);
    tempReg |= (advCfg->ediCfg.lut[12] << CSL_VPS_DEI_DEI_REG7_EDI_LUT12_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[13] << CSL_VPS_DEI_DEI_REG7_EDI_LUT13_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[14] << CSL_VPS_DEI_DEI_REG7_EDI_LUT14_SHIFT);
    tempReg |= (advCfg->ediCfg.lut[15] << CSL_VPS_DEI_DEI_REG7_EDI_LUT15_SHIFT);
    regOvly->DEI_REG7 = tempReg;

    /* Write FMD configurations to DEI_REG10 */
    tempReg = regOvly->DEI_REG10;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG10_FMD_CAF_LINE_THR_MASK |
                 CSL_VPS_DEI_DEI_REG10_FMD_CAF_FIELD_THR_MASK);
    tempReg |= (advCfg->fmdCfg.cafLineThr <<
        CSL_VPS_DEI_DEI_REG10_FMD_CAF_LINE_THR_SHIFT);
    tempReg |= (advCfg->fmdCfg.cafFieldThr <<
        CSL_VPS_DEI_DEI_REG10_FMD_CAF_FIELD_THR_SHIFT);
    regOvly->DEI_REG10 = tempReg;

    /* Write FMD configurations to DEI_REG11 */
    tempReg = regOvly->DEI_REG11;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG11_FMD_CAF_THR_MASK);
    tempReg |= (advCfg->fmdCfg.cafThr <<
        CSL_VPS_DEI_DEI_REG11_FMD_CAF_THR_SHIFT);
    regOvly->DEI_REG11 = tempReg;

    return (VPS_SOK);
}



/**
 *  VpsHal_deiGetAdvConfig
 */
Int32 VpsHal_deiGetAdvConfig(VpsHal_Handle handle,
                             Vps_DeiRdWrAdvCfg *advCfg)
{
    UInt32              tempReg;
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != advCfg));

    regOvly = ((VpsHal_DeiObj *) handle)->baseAddress;

    /* Get MDT configurations from DEI_REG2 */
    tempReg = regOvly->DEI_REG2;
    advCfg->mdtCfg.mvstmMaxCorThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG2_MDT_MVSTMAX_COR_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG2_MDT_MVSTMAX_COR_THR_SHIFT);
    advCfg->mdtCfg.mvCorThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG2_MDT_MV_COR_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG2_MDT_MV_COR_THR_SHIFT);
    advCfg->mdtCfg.sfScThr[0u] =
        ((tempReg & CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR1_MASK) >>
            CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR1_SHIFT);
    advCfg->mdtCfg.sfScThr[1u] =
        ((tempReg & CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR2_MASK) >>
            CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR2_SHIFT);
    advCfg->mdtCfg.sfScThr[2u] =
        ((tempReg & CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR3_MASK) >>
            CSL_VPS_DEI_DEI_REG2_MDT_SF_SC_THR3_SHIFT);

    /* Get EDI configurations from DEI_REG3 */
    tempReg = regOvly->DEI_REG3;
    advCfg->ediCfg.corScaleFactor =
        ((tempReg & CSL_VPS_DEI_DEI_REG3_EDI_COR_SCALE_FACTOR_MASK) >>
            CSL_VPS_DEI_DEI_REG3_EDI_COR_SCALE_FACTOR_SHIFT);
    advCfg->ediCfg.detCorLowerThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG3_EDI_DIR_COR_LOWER_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG3_EDI_DIR_COR_LOWER_THR_SHIFT);
    advCfg->ediCfg.chroma3DCorThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG3_EDI_CHROMA3D_COR_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG3_EDI_CHROMA3D_COR_THR_SHIFT);

    /* Get EDI LUT configurations from DEI_REG4 */
    tempReg = regOvly->DEI_REG4;
    advCfg->ediCfg.lut[0] =
        ((tempReg & CSL_VPS_DEI_DEI_REG4_EDI_LUT0_MASK) >>
            CSL_VPS_DEI_DEI_REG4_EDI_LUT0_SHIFT);
    advCfg->ediCfg.lut[1] =
        ((tempReg & CSL_VPS_DEI_DEI_REG4_EDI_LUT1_MASK) >>
            CSL_VPS_DEI_DEI_REG4_EDI_LUT1_SHIFT);
    advCfg->ediCfg.lut[2] =
        ((tempReg & CSL_VPS_DEI_DEI_REG4_EDI_LUT2_MASK) >>
            CSL_VPS_DEI_DEI_REG4_EDI_LUT2_SHIFT);
    advCfg->ediCfg.lut[3] =
        ((tempReg & CSL_VPS_DEI_DEI_REG4_EDI_LUT3_MASK) >>
            CSL_VPS_DEI_DEI_REG4_EDI_LUT3_SHIFT);

    /* Get EDI LUT configurations from DEI_REG5 */
    tempReg = regOvly->DEI_REG5;
    advCfg->ediCfg.lut[4] =
        ((tempReg & CSL_VPS_DEI_DEI_REG5_EDI_LUT4_MASK) >>
            CSL_VPS_DEI_DEI_REG5_EDI_LUT4_SHIFT);
    advCfg->ediCfg.lut[5] =
        ((tempReg & CSL_VPS_DEI_DEI_REG5_EDI_LUT5_MASK) >>
            CSL_VPS_DEI_DEI_REG5_EDI_LUT5_SHIFT);
    advCfg->ediCfg.lut[6] =
        ((tempReg & CSL_VPS_DEI_DEI_REG5_EDI_LUT6_MASK) >>
            CSL_VPS_DEI_DEI_REG5_EDI_LUT6_SHIFT);
    advCfg->ediCfg.lut[7] =
        ((tempReg & CSL_VPS_DEI_DEI_REG5_EDI_LUT7_MASK) >>
            CSL_VPS_DEI_DEI_REG5_EDI_LUT7_SHIFT);

    /* Get EDI LUT configurations from DEI_REG6 */
    tempReg = regOvly->DEI_REG6;
    advCfg->ediCfg.lut[8] =
        ((tempReg & CSL_VPS_DEI_DEI_REG6_EDI_LUT8_MASK) >>
            CSL_VPS_DEI_DEI_REG6_EDI_LUT8_SHIFT);
    advCfg->ediCfg.lut[9] =
        ((tempReg & CSL_VPS_DEI_DEI_REG6_EDI_LUT9_MASK) >>
            CSL_VPS_DEI_DEI_REG6_EDI_LUT9_SHIFT);
    advCfg->ediCfg.lut[10] =
        ((tempReg & CSL_VPS_DEI_DEI_REG6_EDI_LUT10_MASK) >>
            CSL_VPS_DEI_DEI_REG6_EDI_LUT10_SHIFT);
    advCfg->ediCfg.lut[11] =
        ((tempReg & CSL_VPS_DEI_DEI_REG6_EDI_LUT11_MASK) >>
            CSL_VPS_DEI_DEI_REG6_EDI_LUT11_SHIFT);

    /* Get EDI LUT configurations from DEI_REG7 */
    tempReg = regOvly->DEI_REG7;
    advCfg->ediCfg.lut[12] =
        ((tempReg & CSL_VPS_DEI_DEI_REG7_EDI_LUT12_MASK) >>
            CSL_VPS_DEI_DEI_REG7_EDI_LUT12_SHIFT);
    advCfg->ediCfg.lut[13] =
        ((tempReg & CSL_VPS_DEI_DEI_REG7_EDI_LUT13_MASK) >>
            CSL_VPS_DEI_DEI_REG7_EDI_LUT13_SHIFT);
    advCfg->ediCfg.lut[14] =
        ((tempReg & CSL_VPS_DEI_DEI_REG7_EDI_LUT14_MASK) >>
            CSL_VPS_DEI_DEI_REG7_EDI_LUT14_SHIFT);
    advCfg->ediCfg.lut[15] =
        ((tempReg & CSL_VPS_DEI_DEI_REG7_EDI_LUT15_MASK) >>
            CSL_VPS_DEI_DEI_REG7_EDI_LUT15_SHIFT);

    /* Get FMD configurations from DEI_REG10 */
    tempReg = regOvly->DEI_REG10;
    advCfg->fmdCfg.cafLineThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG10_FMD_CAF_LINE_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG10_FMD_CAF_LINE_THR_SHIFT);
    advCfg->fmdCfg.cafFieldThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG10_FMD_CAF_FIELD_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG10_FMD_CAF_FIELD_THR_SHIFT);

    /* Get FMD configurations from DEI_REG11 */
    tempReg = regOvly->DEI_REG11;
    advCfg->fmdCfg.cafThr =
        ((tempReg & CSL_VPS_DEI_DEI_REG11_FMD_CAF_THR_MASK) >>
            CSL_VPS_DEI_DEI_REG11_FMD_CAF_THR_SHIFT);

    return (VPS_SOK);
}



/**
 *  halDeiInitInstance
 *  Initialize the DEI instance by writing to expert registers and figuring
 *  out the virtual offsets of registers for config overlay memory.
 */
static Int32 halDeiInitInstance(VpsHal_DeiObj *deiObj)
{
    Int32                   retVal;
    CSL_VpsDeiRegsOvly      regOvly;
    VpsHal_DeiExpertConfig  deiExpertConfig = VPSHAL_DEI_DEFAULT_EXPERT_VAL;

    regOvly = deiObj->baseAddress;

    /* Set the expert registers with recommended value */
    retVal = halDeiSetExpertConfig(deiObj, &deiExpertConfig);

    /* Initialize the register offsets for the registers which are used
    * to form the VPDMA configuration overlay */
    deiObj->regOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG0] =
                                    (UInt32 *) &regOvly->DEI_REG0;
    deiObj->regOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG1] =
                                    (UInt32 *) &regOvly->DEI_REG1;
    deiObj->regOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG3] =
                                    (UInt32 *) &regOvly->DEI_REG3;
    deiObj->regOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG8] =
                                    (UInt32 *) &regOvly->DEI_REG8;
    deiObj->regOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG9] =
                                    (UInt32 *) &regOvly->DEI_REG9;
    deiObj->regOffset[VPSHAL_DEI_CONFIGOVLYIDX_DEI_REG10] =
                                    (UInt32 *) &regOvly->DEI_REG10;

    /* Get the size of the overlay for DEI registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    deiObj->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                 deiObj->regOffset,
                                 VPSHAL_DEI_NUM_REG_IDX,
                                 deiObj->virRegOffset);

    return (retVal);
}



/**
 *  halDeiSetExpertConfig
 *  Set the expert registers with expert values.
 */
static Int32 halDeiSetExpertConfig(VpsHal_DeiObj *deiObj,
                                   const VpsHal_DeiExpertConfig *config)
{
    CSL_VpsDeiRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != deiObj));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = deiObj->baseAddress;

    /* Write to DEI expert registers */
    regOvly->DEI_REG2 = config->deiReg2;
    regOvly->DEI_REG3 = config->deiReg3;
    regOvly->DEI_REG4 = config->deiReg4;
    regOvly->DEI_REG5 = config->deiReg5;
    regOvly->DEI_REG6 = config->deiReg6;
    regOvly->DEI_REG7 = config->deiReg7;
    regOvly->DEI_REG10 = config->deiReg10;
    regOvly->DEI_REG11 = config->deiReg11;

    return (VPS_SOK);
}



/**
 *  halDeiMakeDeiReg0Reg
 *  Make DEI REG0 register with given parameters.
 */
static inline UInt32 halDeiMakeDeiReg0Reg(UInt32 initRegVal,
                                          UInt16 width,
                                          UInt16 height,
                                          UInt32 interlaceBypass,
                                          UInt32 fieldFlush,
                                          UInt32 progressiveBypass)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG0_WIDTH_MASK |
                 CSL_VPS_DEI_DEI_REG0_HEIGHT_MASK |
                 CSL_VPS_DEI_DEI_REG0_INTERLACE_BYPASS_MASK |
                 CSL_VPS_DEI_DEI_REG0_FIELD_FLUSH_MASK |
                 CSL_VPS_DEI_DEI_REG0_PROGRESSIVE_BYPASS_MASK);

    /* Set Height and Width */
    tempReg |= (((UInt32) width) << CSL_VPS_DEI_DEI_REG0_WIDTH_SHIFT);
    tempReg |= (((UInt32) height) << CSL_VPS_DEI_DEI_REG0_HEIGHT_SHIFT);

    /* Set interlace bypass field */
    if (TRUE == interlaceBypass)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG0_INTERLACE_BYPASS_MASK;
    }

    /* Set field flush field */
    if (TRUE == fieldFlush)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG0_FIELD_FLUSH_MASK;
    }

    /* Set progressive bypass field */
    if (TRUE == progressiveBypass)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG0_PROGRESSIVE_BYPASS_MASK;
    }

    return (tempReg);
}



/**
 *  halDeiMakeDeiReg1Reg
 *  Make DEI REG1 register with given parameters.
 */
static inline UInt32 halDeiMakeDeiReg1Reg(UInt32 initRegVal,
                                          UInt32 spatMaxBypass,
                                          UInt32 tempMaxBypass)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG1_MDT_TEMPMAX_BYPASS_MASK |
                 CSL_VPS_DEI_DEI_REG1_MDT_SPATMAX_BYPASS_MASK);

    /* Set temporal max bypass field */
    if (TRUE == tempMaxBypass)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG1_MDT_TEMPMAX_BYPASS_MASK;
    }

    /* Set spatial max bypass field */
    if (TRUE == spatMaxBypass)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG1_MDT_SPATMAX_BYPASS_MASK;
    }

    return (tempReg);
}



/**
 *  halDeiMakeDeiReg3Reg
 *  Make DEI REG3 register with given parameters.
 */
static inline UInt32 halDeiMakeDeiReg3Reg(UInt32 initRegVal,
                                          UInt32 inpMode,
                                          UInt32 tempInpEnable,
                                          UInt32 tempInpChromaEnable)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG3_EDI_INP_MODE_MASK |
                 CSL_VPS_DEI_DEI_REG3_EDI_ENABLE_3D_MASK |
                 CSL_VPS_DEI_DEI_REG3_EDI_CHROMA_3D_ENABLE_MASK);

    /* Set EDI interpolation Mode */
    tempReg |= (((UInt32) inpMode) <<
                   CSL_VPS_DEI_DEI_REG3_EDI_INP_MODE_SHIFT);

    /* Set temporal interpolation for luma field */
    if (TRUE == tempInpEnable)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG3_EDI_ENABLE_3D_MASK;
    }

    /* Set temporal interpolation for chroma field */
    if (TRUE == tempInpChromaEnable)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG3_EDI_CHROMA_3D_ENABLE_MASK;
    }

    return (tempReg);
}



/**
 *  halDeiMakeDeiReg8Reg
 *  Make DEI REG8 register with given parameters.
 */
static inline UInt32 halDeiMakeDeiReg8Reg(UInt32 initRegVal,
                                          UInt16 windowMinx,
                                          UInt16 windowMaxx,
                                          UInt32 window)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MINX_MASK |
                 CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MAXX_MASK |
                 CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_ENABLE_MASK);

    /* Set FMD window fields - horizontal limits */
    tempReg |= (((UInt32) windowMinx) <<
                   CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MINX_SHIFT);
    tempReg |= (((UInt32) windowMaxx) <<
                   CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_MAXX_SHIFT);

    /* Set enable FMD window fields */
    if (TRUE == window)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG8_FMD_WINDOW_ENABLE_MASK;
    }

    return (tempReg);
}



/**
 *  halDeiMakeDeiReg9Reg
 *  Make DEI REG9 register with given parameters.
 */
static inline UInt32 halDeiMakeDeiReg9Reg(UInt32 initRegVal,
                                          UInt16 windowMiny,
                                          UInt16 windowMaxy)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MINY_MASK |
                 CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MAXY_MASK);

    /* Set FMD window fields - vertical limits */
    tempReg |= (((UInt32) windowMiny) <<
                   CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MINY_SHIFT);
    tempReg |= (((UInt32) windowMaxy) <<
                   CSL_VPS_DEI_DEI_REG9_FMD_WINDOW_MAXY_SHIFT);

    return (tempReg);
}



/**
 *  halDeiMakeDeiReg10Reg
 *  Make DEI REG10 register with given parameters.
 */
static inline UInt32 halDeiMakeDeiReg10Reg(UInt32 initRegVal,
                                           UInt32 filmMode,
                                           UInt32 bed,
                                           UInt32 lock,
                                           UInt32 jamDir)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_DEI_REG10_FMD_ENABLE_MASK |
                 CSL_VPS_DEI_DEI_REG10_FMD_LOCK_MASK |
                 CSL_VPS_DEI_DEI_REG10_FMD_JAM_DIR_MASK |
                 CSL_VPS_DEI_DEI_REG10_FMD_BED_ENABLE_MASK);

    /* Set enable film mode field */
    if (TRUE == filmMode)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG10_FMD_ENABLE_MASK;
    }

    /* Set lock to film mode field */
    if (TRUE == lock)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG10_FMD_LOCK_MASK;
    }

    /* Set jamming direction field */
    if (VPS_DEI_FMDJAMDIR_NEXT_FLD == jamDir)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG10_FMD_JAM_DIR_MASK;
    }

    /* Set enable bad edit detection field */
    if (TRUE == bed)
    {
        tempReg |= CSL_VPS_DEI_DEI_REG10_FMD_BED_ENABLE_MASK;
    }

    return (tempReg);
}
