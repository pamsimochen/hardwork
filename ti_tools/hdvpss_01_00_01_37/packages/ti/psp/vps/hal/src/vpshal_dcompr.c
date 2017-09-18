/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_dcompr.c
 *
 *  \brief VPS DCOMPR HAL file.
 *  This file implements the HAL APIs of the VPS Decompressor.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/cslr/cslr_vps_decompress.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_dcompr.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum VpsHal_DcomprConfigOvlyIdx
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
    VPSHAL_DCOMPR_CONFIGOVLYIDX_STATUS = 0,
    VPSHAL_DCOMPR_CONFIGOVLYIDX_UNIT_SETTINGS,
    VPSHAL_DCOMPR_NUM_REG_IDX               /* This should be the last enum */
} VpsHal_DcomprConfigOvlyIdx;

/** \brief Unit size should be a multiple of 16. */
#define VPSHAL_DCOMPR_UNIT_SIZE_ALIGN   (16u)

/*
 *  Macros for maximum/minimum value possible for some of the bit fields used.
 */
/** \brief Minimum unit size. */
#define VPSHAL_DCOMPR_MIN_UNIT_SIZE     (32u)
/** \brief Maximum unit size. */
#define VPSHAL_DCOMPR_MAX_UNIT_SIZE     (80u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_DcomprObj
 *  \brief Per instance information about each DCOMPR module.
 */
typedef struct
{
    UInt32                  instId;
    /**< Instance number/ID. */
    UInt32                  openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    CSL_VpsDecompressRegsOvly baseAddress;
    /**< Register overlay pointer. */
    UInt32                  configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay memory
         required. */
    UInt32                 *regOffset[VPSHAL_DCOMPR_NUM_REG_IDX];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA. */
    UInt32                  virRegOffset[VPSHAL_DCOMPR_NUM_REG_IDX];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to regOffset member. */
} VpsHal_DcomprObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  DCOMPR objects - Module variable to store information about each
 *  Compressor instance.
 *  Note: If the number of DCOMPR instance increase, then VPSHAL_DCOMPR_MAX_INST
 *  macro should be changed accordingly.
 */
static VpsHal_DcomprObj DcomprObjects[VPSHAL_DCOMPR_MAX_INST];


/* ========================================================================== */
/*                          Function Definitions                              */
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
                        Ptr arg)
{
    UInt32                      instCnt;
    VpsHal_DcomprObj           *dcomprObj;
    Int32                       retVal = VPS_SOK;
    CSL_VpsDecompressRegsOvly   regOvly;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_DCOMPR_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    /* Initialize DCOMPR Objects to zero */
    VpsUtils_memset(DcomprObjects, 0, sizeof(DcomprObjects));

    for (instCnt = 0u; instCnt < numInstances; instCnt++)
    {
        GT_assert(VpsHalTrace,
            (initParams[instCnt].instId < VPSHAL_DCOMPR_MAX_INST));

        dcomprObj = &DcomprObjects[instCnt];
        dcomprObj->instId = initParams[instCnt].instId;
        dcomprObj->baseAddress =
            (CSL_VpsDecompressRegsOvly) initParams[instCnt].baseAddress;

        /* Initialize the register offsets for the registers which are used
         * to form the VPDMA configuration overlay */
        regOvly = dcomprObj->baseAddress;
        dcomprObj->regOffset[VPSHAL_DCOMPR_CONFIGOVLYIDX_STATUS] =
            (UInt32 *) &regOvly->STATUS;
        dcomprObj->regOffset[VPSHAL_DCOMPR_CONFIGOVLYIDX_UNIT_SETTINGS] =
            (UInt32 *) &regOvly->UNIT_SETTINGS;

        /* Get the size of the overlay for DCOMPR registers and the relative
         * virtual offset for the above registers when VPDMA config register
         * overlay is formed */
        dcomprObj->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                        dcomprObj->regOffset,
                                        VPSHAL_DCOMPR_NUM_REG_IDX,
                                        dcomprObj->virRegOffset);
    }

    return (retVal);
}



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
Int32 VpsHal_dcomprDeInit(Ptr arg)
{
    return (VPS_SOK);
}



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
VpsHal_Handle VpsHal_dcomprOpen(UInt32 instId)
{
    UInt32              cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;

    /* Check if instance number is valid */
    GT_assert(VpsHalTrace, (instId < VPSHAL_DCOMPR_MAX_INST));

    for (cnt = 0u; cnt < VPSHAL_DCOMPR_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (instId == DcomprObjects[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0u == DcomprObjects[cnt].openCnt)
            {
                handle = (VpsHal_Handle) &DcomprObjects[cnt];
                DcomprObjects[cnt].openCnt++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);
            break;
        }
    }

    return (handle);
}



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
Int32 VpsHal_dcomprClose(VpsHal_Handle handle)
{
    Int32               retVal = VPS_EFAIL;
    UInt32              cookie;
    VpsHal_DcomprObj   *dcomprObj;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    dcomprObj = (VpsHal_DcomprObj *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (dcomprObj->openCnt > 0u)
    {
        dcomprObj->openCnt--;
        retVal = VPS_SOK;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



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
UInt32 VpsHal_dcomprGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32          configOvlySize = 0u;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((VpsHal_DcomprObj *) handle)->configOvlySize;

    return configOvlySize;
}



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
Int32 VpsHal_dcomprCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int32           retVal;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    retVal = VpsHal_vpdmaCreateRegOverlay(
                 ((VpsHal_DcomprObj *) handle)->regOffset,
                 VPSHAL_DCOMPR_NUM_REG_IDX,
                 configOvlyPtr);

    return (retVal);
}



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
                             Ptr configOvlyPtr)
{
    Int32                       retVal = VPS_SOK;
    VpsHal_DcomprObj           *dcomprObj;
    UInt32                      status, unitSetting;
    CSL_VpsDecompressRegsOvly   regOvly;
    UInt32                     *virRegOffset;
    UInt8                       unitPerLine = 0u;
    UInt8                       unitSize = 0u;
    UInt8                       tempUnitSize;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    dcomprObj = (VpsHal_DcomprObj *) handle;
    regOvly = dcomprObj->baseAddress;

    /* Calculate number of pixel per unit. Unit size is multiple of 16
     * between 32 and 80. And width should be an integral multiple of
     * unit size. Start with maximum unit size for better SNR. */
    tempUnitSize = VPSHAL_DCOMPR_MAX_UNIT_SIZE;
    while ((tempUnitSize >= VPSHAL_DCOMPR_MIN_UNIT_SIZE) &&
           (tempUnitSize <= VPSHAL_DCOMPR_MAX_UNIT_SIZE))
    {
        if (((config->width / tempUnitSize) * tempUnitSize) == config->width)
        {
            unitSize = tempUnitSize;
            unitPerLine = (UInt8) (config->width / tempUnitSize);
            break;
        }
        tempUnitSize -= VPSHAL_DCOMPR_UNIT_SIZE_ALIGN;
    }

    /*
     * Make status register.
     */
    /* Reset the bit fields which are being set */
    status = regOvly->STATUS;
    status &= ~(CSL_VPS_DECOMPRESS_STATUS_ENABLE_MASK);
    /* Set enable field */
    if (TRUE == config->enable)
    {
        status |= CSL_VPS_DECOMPRESS_STATUS_ENABLE_MASK;
    }

    /* Check for unit size error only if compressor needs to be enabled */
    if (TRUE == config->enable)
    {
        if (0u != unitSize)
        {
            /* Unit size should be multiple of 16 */
            GT_assert(VpsHalTrace,
                !(unitSize & (VPSHAL_DCOMPR_UNIT_SIZE_ALIGN - 1u)));
        }
        else
        {
            GT_0trace(VpsHalTrace, GT_ERR,
                "Width did not match any of the required alignment!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        /*
         * Make unit settings register.
         */
        /* Reset the bit fields which are being set */
        unitSetting = regOvly->UNIT_SETTINGS;
        unitSetting &= ~(CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_SIZE_MASK |
                         CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_PER_LINE_MASK);
        unitSetting |= (((UInt32) unitSize)
            << CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_SIZE_SHIFT);
        unitSetting |= (((UInt32) unitPerLine)
            << CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_PER_LINE_SHIFT);

        if (NULL == configOvlyPtr)
        {
            /* Write to the actual register */
            regOvly->STATUS = status;
            regOvly->UNIT_SETTINGS = unitSetting;
        }
        else            /* Update in config overlay memory */
        {
            /* Write to config register overlay at the correct offset. Since
             * the offset is in words, it can be directly added to the pointer
             * so that (UInt32 *) addition will result in proper offset value */
            virRegOffset = (UInt32 *) configOvlyPtr +
                dcomprObj->virRegOffset[VPSHAL_DCOMPR_CONFIGOVLYIDX_STATUS];
            *virRegOffset = status;
            virRegOffset = (UInt32 *) configOvlyPtr +
                dcomprObj->virRegOffset
                [VPSHAL_DCOMPR_CONFIGOVLYIDX_UNIT_SETTINGS];
            *virRegOffset = unitSetting;
        }
    }

    return (retVal);
}
