/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_cig.c
 *
 *  \brief VPS Constrained Image Generator HAL file.
 *  This file implements the HAL APIs of the VPS CIG.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/cslr/cslr_vps_cig.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_cig.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum VpsHal_CigConfigOvlyIdx
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
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG0 = 0,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG1,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG2,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG3,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG4,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG5,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG6,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG7,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG8,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG9,
    VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG10,
    VPSHAL_CIG_NUM_REG_IDX              /* This should be the last enum */
} VpsHal_CigConfigOvlyIdx;

/**
 *  enum VpsHal_CigRtConfigOvlyIdx
 *  \brief Enums for the runtime configuration overlay register offsets - used
 *  as array indices to register offset array. These enums defines the
 *  registers that will be used to form the runtime configuration register
 *  overlay.
 *  This in turn is used by client drivers to configure registers using
 *  VPDMA for per frame configuration change.
 *
 *  This is separated from main config overlay since only some of the
 *  registers are intended to be changed at runtime.
 *
 *  Note: If in future it is decided to remove or add some register, this
 *  enum should be modified accordingly.
 *  And make sure that the values of these enums are in ascending order
 *  and it starts from 0.
 */
typedef enum
{
    VPSHAL_CIG_RT_CONFIGOVLYIDX_CIG_REG7 = 0,
    VPSHAL_CIG_RT_CONFIGOVLYIDX_CIG_REG8,
    VPSHAL_CIG_NUM_RT_REG_IDX           /* This should be the last enum */
} VpsHal_CigRtConfigOvlyIdx;

typedef enum
{
    VPSHAL_CIG_PIP_ENABLE_CONFIGOVLYIDX_REG0 = 0,
    VPSHAL_CIG_NUM_PIP_ENABLE_REG_IDX   /* This should be the last enum */
} VpsHal_CigPipEnableConfigOvlyIdx;

/**
 *  \brief Macros for maximum value possible for some of the bit fields used.
 */
#define VPSHAL_CIG_MAX_CIG_WIDTH        (1920u)
#define VPSHAL_CIG_MAX_CIG_HEIGHT                                              \
                    (CSL_VPS_CIG_CIG_REG1_DISP_H_MASK >>                       \
                     CSL_VPS_CIG_CIG_REG1_DISP_H_SHIFT)
#define VPSHAL_CIG_MAX_PIP_DISP_WIDTH   (1920u)
#define VPSHAL_CIG_MAX_PIP_DISP_HEIGHT                                         \
                    (CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_MASK >>                   \
                     CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_SHIFT)
#define VPSHAL_CIG_MAX_PIP_X_POS                                               \
                    (CSL_VPS_CIG_CIG_REG7_PIP_X_MASK >>                        \
                     CSL_VPS_CIG_CIG_REG7_PIP_X_SHIFT)
#define VPSHAL_CIG_MAX_PIP_Y_POS                                               \
                    (CSL_VPS_CIG_CIG_REG7_PIP_Y_MASK >>                        \
                     CSL_VPS_CIG_CIG_REG7_PIP_Y_SHIFT)
#define VPSHAL_CIG_MAX_PIP_WIDTH                                               \
                    (CSL_VPS_CIG_CIG_REG8_PIP_W_MASK >>                        \
                     CSL_VPS_CIG_CIG_REG8_PIP_W_SHIFT)
#define VPSHAL_CIG_MAX_PIP_HEIGHT                                              \
                    (CSL_VPS_CIG_CIG_REG8_PIP_H_MASK >>                        \
                     CSL_VPS_CIG_CIG_REG8_PIP_H_SHIFT)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_CigObj
 *  \brief Per instance information about each CIG module.
 */
typedef struct
{
    UInt32              instId;
    /**< Instance number/ID */
    UInt32              openCnt;
    /**< Counter to keep track of number of open calls for an instance */
    CSL_VpsCigRegsOvly  baseAddress;
    /**< Register overlay pointer */

    UInt32              configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay memory
         required */
    UInt32             *regOffset[VPSHAL_CIG_NUM_REG_IDX];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA */
    UInt32              virRegOffset[VPSHAL_CIG_NUM_REG_IDX];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to regOffset member */

    UInt32              rtConfigOvlySize;
    /**< Size (in bytes) of runtime VPDMA register
         configuration overlay memory required */
    UInt32             *rtRegOffset[VPSHAL_CIG_NUM_RT_REG_IDX];
     /**< Array of register offsets for all the runtime registers that
          needs to be programmed through VPDMA */
    UInt32              rtVirRegOffset[VPSHAL_CIG_NUM_RT_REG_IDX];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to runtime regOffset member */

    UInt32              pipEnableConfigOvlySize;
    /**< Size (in bytes) of runtime VPDMA register
         configuration overlay memory required */
    UInt32             *pipEnableRegOffset[VPSHAL_CIG_NUM_PIP_ENABLE_REG_IDX];
     /**< Array of register offsets for all the runtime registers that
          needs to be programmed through VPDMA */
    UInt32              pipEnableVirRegOffset[VPSHAL_CIG_NUM_PIP_ENABLE_REG_IDX];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to runtime regOffset member */
    UInt32              cigCfg0Reg;
} VpsHal_CigObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 VpsHal_cigSetPipConfig(VpsHal_Handle handle,
                                    const VpsHal_CigPipConfig *config,
                                    Ptr configOvlyPtr);

static Int32 VpsHal_cigGetPipConfig(VpsHal_Handle handle,
                                    VpsHal_CigPipConfig *config);

static Int32 halCigInitInstance(VpsHal_CigObj *cigObj);

static inline UInt32 halCigMakeCigReg0Reg(UInt32 initRegVal,
                                          UInt32 enableCig,
                                          UInt32 nonCtrInterlace,
                                          UInt32 ctrInterlace,
                                          UInt32 pipInterlace,
                                          UInt32 vertDecimation,
                                          UInt32 fldRepeat,
                                          UInt32 enablePip,
                                          VpsHal_CigPipWinSz pipWinSz);

static inline UInt32 halCigMakeCigReg1Reg(UInt32 initRegVal,
                                          UInt16 width,
                                          UInt16 height);

static inline UInt32 halCigMakeCigReg2Reg(UInt32 initRegVal,
                                          UInt32 transparency,
                                          VpsHal_CigTransMask mask,
                                          UInt32 alphaBlending,
                                          UInt8 alphaValue);

static inline UInt32 halCigMakeCigReg3Reg(UInt32 initRegVal,
                                          UInt8 rTransColor,
                                          UInt8 gTransColor,
                                          UInt8 bTransColor);

static inline UInt32 halCigMakeCigReg4Reg(UInt32 initRegVal,
                                          UInt32 transparency,
                                          VpsHal_CigTransMask mask,
                                          UInt32 alphaBlending,
                                          UInt8 alphaValue);

static inline UInt32 halCigMakeCigReg5Reg(UInt32 initRegVal,
                                          UInt8 rTransColor,
                                          UInt8 gTransColor,
                                          UInt8 bTransColor);

static inline UInt32 halCigMakeCigReg6Reg(UInt32 initRegVal,
                                          UInt16 dispWidth,
                                          UInt16 dispHeight);

static inline UInt32 halCigMakeCigReg7Reg(UInt32 initRegVal,
                                          UInt16 xPos,
                                          UInt16 yPos);

static inline UInt32 halCigMakeCigReg8Reg(UInt32 initRegVal,
                                          UInt16 pipWidth,
                                          UInt16 pipHeight);

static inline UInt32 halCigMakeCigReg9Reg(UInt32 initRegVal,
                                          UInt32 transparency,
                                          VpsHal_CigTransMask mask,
                                          UInt32 alphaBlending,
                                          UInt8 alphaValue);

static inline UInt32 halCigMakeCigReg10Reg(UInt32 initRegVal,
                                           UInt8 rTransColor,
                                           UInt8 gTransColor,
                                           UInt8 bTransColor);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  CIG objects - Module variable to store information about each CIG instance.
 *  Note: If the number of CIG instance increase, then VPSHAL_CIG_MAX_INST
 *  macro should be changed accordingly.
 */
static VpsHal_CigObj CigObjects[VPSHAL_CIG_MAX_INST];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_cigInit
 *  \brief CIG HAL init function
 *  Initializes CIG objects, gets the register overlay offsets for CIG
 *  registers.
 *  This function should be called before calling any of CIG HAL API's
 *
 *  \param numInstances Number of instance objects to be initialized
 *  \param initParams   Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances
 *  \param arg          Not used currently.Meant for future purpose
 *
 *  \return             Returns VPS_SOK on success else returns error value
 */
Int VpsHal_cigInit(UInt32 numInstances,
                   const VpsHal_CigInitParams *initParams,
                   Ptr arg)
{
    Int             instCnt;
    VpsHal_CigObj  *cigObj;
    Int             retVal = VPS_SOK;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_CIG_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    /* Initialize CIG Objects to zero */
    VpsUtils_memset(CigObjects, 0, sizeof(CigObjects));

    for (instCnt = 0; instCnt < numInstances; instCnt++)
    {
        GT_assert(VpsHalTrace,
            (initParams[instCnt].instId < VPSHAL_CIG_MAX_INST));

        cigObj = &CigObjects[instCnt];
        cigObj->instId = initParams[instCnt].instId;
        cigObj->baseAddress =
            (CSL_VpsCigRegsOvly) initParams[instCnt].baseAddress;

        /* Initialize the instance */
        retVal += halCigInitInstance(cigObj);
    }

    return (retVal);
}



/**
 *  VpsHal_cigDeInit
 *  \brief CIG HAL exit function.
 *  Currently this function does not do anything.
 *
 *  \param arg          Not used currently. Meant for future purpose
 *
 *  \return             Returns VPS_SOK on success else returns error value
 */
Int VpsHal_cigDeInit(Ptr arg)
{
    return (VPS_SOK);
}



/**
 *  VpsHal_cigOpen
 *  \brief Returns the handle to the requested CIG instance.
 *  This function should be called prior to calling any of the CIG HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId   Requested CIG instance.
 *
 *  \return         Returns CIG instance handle on success else
 *                  returns NULL
 */
VpsHal_Handle VpsHal_cigOpen(UInt32 instId)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;

    /* Check if instance number is valid */
    GT_assert(VpsHalTrace, (instId < VPSHAL_CIG_MAX_INST));

    for (cnt = 0; cnt < VPSHAL_CIG_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (instId == CigObjects[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0 == CigObjects[cnt].openCnt)
            {
                handle = (VpsHal_Handle) &CigObjects[cnt];
                CigObjects[cnt].openCnt++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }

    return handle;
}



/**
 *  VpsHal_cigClose
 *  \brief Closes the CIG HAL instance.
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param  handle  Valid handle returned by VpsHal_cigOpen function
 *
 *  \return         Returns VPS_SOK on success else returns error value
 */
Int VpsHal_cigClose(VpsHal_Handle handle)
{
    Int             retVal = VPS_EFAIL;
    UInt32          cookie;
    VpsHal_CigObj  *cigObj;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    cigObj = (VpsHal_CigObj *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (cigObj->openCnt > 0)
    {
        cigObj->openCnt--;
        retVal = VPS_SOK;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  VpsHal_cigGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the CIG registers using VPDMA
 *  config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the CIG registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context
 *  (multiple instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay
 *  one-time and submit this to VPDMA everytime when context changes.
 *  Thus saving valuable CPU in programming the VPS registers
 *
 * \param  handle   Instance handle
 *
 * \return return   Returns the size of the config overlay memory
 */
UInt32 VpsHal_cigGetConfigOvlySize(VpsHal_Handle handle,
                                   VpsHal_CigOvlyType ovlyType)
{
    UInt32          configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    if (VPSHAL_COT_CONFIG == ovlyType)
    {
        configOvlySize = ((VpsHal_CigObj *) handle)->configOvlySize;
    }
    else if (VPSHAL_COT_PIP_ENABLE_CONFIG == ovlyType)
    {
        configOvlySize = ((VpsHal_CigObj *) handle)->pipEnableConfigOvlySize;
    }
    else
    {
        configOvlySize = ((VpsHal_CigObj *) handle)->rtConfigOvlySize;
    }

    return (configOvlySize);
}



/**
 *  VpsHal_cigCreateConfigOvly
 *  \brief Creates the CIG register configuration overlay using VPDMA helper
 *  function
 *  This function does not initialize the overlay with CIG configuration.
 *  It is  the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_cigSetConfig function before submitting the same
 *  to the VPDMA for register configuration at runtime
 *
 *  \param handle           Instance handle
 *  \param configOvlyPtr    Pointer to the overlay memory where the
 *                          overlay is formed. The memory for the overlay
 *                          should be allocated by client driver.
 *                          This parameter should be non-NULL
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_cigCreateConfigOvly(VpsHal_Handle handle,
                                 VpsHal_CigOvlyType ovlyType,
                                 Ptr configOvlyPtr)
{
    Int32           retVal;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Return the value already got in init function */
    if (VPSHAL_COT_CONFIG == ovlyType)
    {
        /* Create the register overlay */
        retVal = VpsHal_vpdmaCreateRegOverlay(
                     ((VpsHal_CigObj *) handle)->regOffset,
                     VPSHAL_CIG_NUM_REG_IDX,
                     configOvlyPtr);
    }
    else if (VPSHAL_COT_PIP_ENABLE_CONFIG == ovlyType)
    {
        /* Create the register overlay */
        retVal = VpsHal_vpdmaCreateRegOverlay(
                     ((VpsHal_CigObj *) handle)->pipEnableRegOffset,
                     VPSHAL_CIG_NUM_PIP_ENABLE_REG_IDX,
                     configOvlyPtr);
    }
    else
    {
        /* Create the register overlay */
        retVal = VpsHal_vpdmaCreateRegOverlay(
                     ((VpsHal_CigObj *) handle)->rtRegOffset,
                     VPSHAL_CIG_NUM_RT_REG_IDX,
                     configOvlyPtr);
    }

    return (retVal);
}



/**
 *  VpsHal_cigSetConfig
 *  \brief Sets the entire CIG configuration to either the actual CIG registers
 *  or to the configuration overlay memory.
 *
 *  This function configures the entire CIG registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual CIG MMR or to configuration overlay memory
 *
 *  \param handle           Instance Handle
 *  \param config           Pointer to VpsHal_CigConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the configuration
 *                          is written to the actual CIG registers. Otherwise
 *                          the configuration is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_cigSetConfig(VpsHal_Handle handle,
                          const VpsHal_CigConfig *config,
                          Ptr configOvlyPtr)
{
    Int32               retVal;
    VpsHal_CigObj      *cigObj;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->width <= VPSHAL_CIG_MAX_CIG_WIDTH));
    GT_assert(VpsHalTrace, (config->height <= VPSHAL_CIG_MAX_CIG_HEIGHT));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Make register */
    tempReg = halCigMakeCigReg0Reg(
                  cigObj->cigCfg0Reg,
                  config->enableCig,
                  config->nonCtrInterlace,
                  config->ctrInterlace,
                  config->pipInterlace,
                  config->vertDecimation,
                  config->fldRepeat,
                  config->enablePip,
                  config->pipWinSz);
    cigObj->cigCfg0Reg = tempReg;
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG0];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halCigMakeCigReg1Reg(
                  regOvly->CIG_REG1,
                  config->width,
                  config->height);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG1 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG1];
        *virRegOffset = tempReg;
    }

    /* Call individual functions to set the configuration */
    retVal = VpsHal_cigSetPipConfig(handle, &config->pipConfig, configOvlyPtr);
    retVal += VpsHal_cigSetHdmiTransConfig(
                  handle,
                  &config->hdmi,
                  configOvlyPtr);
    retVal += VpsHal_cigSetHdcompTransConfig(
                  handle,
                  &config->hdcomp,
                  configOvlyPtr);
    retVal += VpsHal_cigSetPipTransConfig(
                  handle,
                  &config->pip,
                  configOvlyPtr);

    return (retVal);
}


/**
 *  VpsHal_cigGetConfig
 *  \brief Gets the entire CIG configuration from the actual CIG registers.
 *  This function gets the entire CIG configuration.
 *
 *  \param handle   Instance handle
 *  \param config   Pointer to VpsHal_CigConfig structure to be filled with
 *                  register configurations.
 *                  This parameter should be non-NULL.
 *
 *  \return         Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_cigGetConfig(VpsHal_Handle handle, VpsHal_CigConfig *config)
{
    Int32               retVal;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    VpsHal_CigObj      *cigObj;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Get CIG configurations. Since local variable is used for
       storing configuration for PIP and Main window, getting
       configuration from local variable. */
    tempReg = cigObj->cigCfg0Reg;

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_CIG_EN_MASK))
    {
        config->enableCig = TRUE;
    }
    else            /* CIG disabled */
    {
        config->enableCig = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_P2I_EN_MASK))
    {
        config->nonCtrInterlace = TRUE;
    }
    else            /* Non-constrained interlace output disabled */
    {
        config->nonCtrInterlace = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_CI_P2I_EN_MASK))
    {
        config->ctrInterlace = TRUE;
    }
    else            /* Constrained interlace output disabled */
    {
        config->ctrInterlace = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_PIP_P2I_EN_MASK))
    {
        config->pipInterlace = TRUE;
    }
    else            /* PIP interlace output disabled */
    {
        config->pipInterlace = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_CI_VDEC_EN_MASK))
    {
        config->vertDecimation = TRUE;
    }
    else            /* Vertical decimation disabled */
    {
        config->vertDecimation = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_CI_FIELD_RPT_EN_MASK))
    {
        config->fldRepeat = TRUE;
    }
    else            /* Field repeat disabled */
    {
        config->fldRepeat = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK))
    {
        config->enablePip = TRUE;
    }
    else            /* PIP path enabled */
    {
        config->enablePip = FALSE;
    }

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_MASK))
    {
        config->pipWinSz = VPSHAL_CIG_PWS_FULL_SIZE;
    }
    else            /* Sub-window size for PIP */
    {
        config->pipWinSz = VPSHAL_CIG_PWS_SUB_WINDOW;
    }

    /* Get CIG width and height configurations */
    tempReg = regOvly->CIG_REG1;
    config->width = ((tempReg & CSL_VPS_CIG_CIG_REG1_DISP_W_MASK) >>
                          CSL_VPS_CIG_CIG_REG1_DISP_W_SHIFT);
    config->height = ((tempReg & CSL_VPS_CIG_CIG_REG1_DISP_H_MASK) >>
                          CSL_VPS_CIG_CIG_REG1_DISP_H_SHIFT);

    /* Call individual functions to get the configuration */
    retVal = VpsHal_cigGetPipConfig(handle, &config->pipConfig);
    retVal += VpsHal_cigGetHdmiTransConfig(handle, &config->hdmi);
    retVal += VpsHal_cigGetHdcompTransConfig(handle, &config->hdcomp);
    retVal += VpsHal_cigGetPipTransConfig(handle, &config->pip);

    return (retVal);
}



/**
 *  VpsHal_cigSetRtConfig
 *  \brief Sets the runtime CIG configuration to the configuration
 *  overlay memory.
 *
 *  This function configures only the runtime CIG registers.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_CigConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          This parameter should be non-NULL.
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_cigSetRtConfig(VpsHal_Handle handle,
                            const VpsHal_CigRtConfig *config,
                            Ptr configOvlyPtr)
{
    Int32                       retVal = VPS_SOK;
    VpsHal_CigObj              *cigObj;
    UInt32                      tempReg;
    CSL_VpsCigRegsOvly          regOvly;
    UInt32                     *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->xPos <= VPSHAL_CIG_MAX_PIP_X_POS));
    GT_assert(VpsHalTrace, (config->yPos <= VPSHAL_CIG_MAX_PIP_Y_POS));
    GT_assert(VpsHalTrace, (config->pipWidth <= VPSHAL_CIG_MAX_PIP_WIDTH));
    GT_assert(VpsHalTrace, (config->pipHeight <= VPSHAL_CIG_MAX_PIP_HEIGHT));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Make register */
    tempReg = halCigMakeCigReg7Reg(
                  regOvly->CIG_REG7,
                  config->xPos,
                  config->yPos);
    /* Write to runtime config register overlay at the correct offset.
     * Since the offset is in words, it can be directly added to the pointer
     * so that (UInt32 *) addition will result in proper offset value */
    virRegOffset =
        (UInt32 *) configOvlyPtr +
        cigObj->rtVirRegOffset[VPSHAL_CIG_RT_CONFIGOVLYIDX_CIG_REG7];
    *virRegOffset = tempReg;

    /* Make register */
    tempReg = halCigMakeCigReg8Reg(
                  regOvly->CIG_REG8,
                  config->pipWidth,
                  config->pipHeight);
    /* Write to runtime config register overlay at the correct offset.
     * Since the offset is in words, it can be directly added to the pointer
     * so that (UInt32 *) addition will result in proper offset value */
    virRegOffset =
        (UInt32 *) configOvlyPtr +
        cigObj->rtVirRegOffset[VPSHAL_CIG_RT_CONFIGOVLYIDX_CIG_REG8];
    *virRegOffset = tempReg;

    return (retVal);
}



Int32 VpsHal_cigEnablePip(VpsHal_Handle handle,
                          UInt32 isEnable,
                          Ptr configOvlyPtr)
{
    VpsHal_CigObj           *cigObj = NULL;
    CSL_VpsCigRegsOvly       regOvly = NULL;
    UInt32                  *virRegOffset = NULL;

    cigObj = (VpsHal_CigObj *)handle;
    GT_assert(VpsHalTrace, (NULL != cigObj));
    regOvly = cigObj->baseAddress;
    GT_assert(VpsHalTrace, (NULL != regOvly));

    if (NULL == configOvlyPtr)
    {
        if (TRUE == isEnable)
        {
            cigObj->cigCfg0Reg |= CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK;
            cigObj->cigCfg0Reg &= (~CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_MASK);
            regOvly->CIG_REG0 = cigObj->cigCfg0Reg;
        }
        else
        {
            cigObj->cigCfg0Reg &= (~CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK);
            regOvly->CIG_REG0 = cigObj->cigCfg0Reg;
        }
    }
    else
    {
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->pipEnableVirRegOffset[VPSHAL_CIG_PIP_ENABLE_CONFIGOVLYIDX_REG0];
        if (TRUE == isEnable)
        {
            cigObj->cigCfg0Reg |= CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK;
            cigObj->cigCfg0Reg &= (~CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_MASK);
            *virRegOffset = cigObj->cigCfg0Reg;
        }
        else
        {
            cigObj->cigCfg0Reg &= (~CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK);
            *virRegOffset = cigObj->cigCfg0Reg;
        }
    }

    return (VPS_SOK);
}



Int32 VpsHal_cigEnableMain(VpsHal_Handle handle,
                           UInt32 isEnable,
                           Ptr configOvlyPtr)
{
    VpsHal_CigObj           *cigObj = NULL;
    CSL_VpsCigRegsOvly       regOvly = NULL;
    UInt32                  *virRegOffset = NULL;

    cigObj = (VpsHal_CigObj *)handle;
    GT_assert(VpsHalTrace, (NULL != cigObj));
    regOvly = cigObj->baseAddress;
    GT_assert(VpsHalTrace, (NULL != regOvly));

    if (NULL == configOvlyPtr)
    {
        regOvly->CIG_REG0 = cigObj->cigCfg0Reg;
    }
    else
    {
        /* Since register for enabling pip and main input is same, using
           the same overlay offset for the main window as well */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->pipEnableVirRegOffset[VPSHAL_CIG_PIP_ENABLE_CONFIGOVLYIDX_REG0];
        *virRegOffset = cigObj->cigCfg0Reg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_cigSetPipConfig
 *  \brief Sets the CIG PIP configuration to either the actual CIG registers
 *  or to the configuration overlay memory.
 */
static Int32 VpsHal_cigSetPipConfig(VpsHal_Handle handle,
                                    const VpsHal_CigPipConfig *config,
                                    Ptr configOvlyPtr)
{
    Int32               retVal = VPS_SOK;
    VpsHal_CigObj      *cigObj;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace,
        (config->dispWidth <= VPSHAL_CIG_MAX_PIP_DISP_WIDTH));
    GT_assert(VpsHalTrace,
        (config->dispHeight <= VPSHAL_CIG_MAX_PIP_DISP_HEIGHT));
    GT_assert(VpsHalTrace,
        (config->xPos <= VPSHAL_CIG_MAX_PIP_X_POS));
    GT_assert(VpsHalTrace,
        (config->yPos <= VPSHAL_CIG_MAX_PIP_Y_POS));
    GT_assert(VpsHalTrace,
        (config->pipWidth <= VPSHAL_CIG_MAX_PIP_WIDTH));
    GT_assert(VpsHalTrace,
        (config->pipHeight <= VPSHAL_CIG_MAX_PIP_HEIGHT));
    GT_assert(VpsHalTrace,
        ((config->xPos + config->pipWidth) <= config->dispWidth));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Make register */
    tempReg = halCigMakeCigReg6Reg(
                  regOvly->CIG_REG6,
                  config->dispWidth,
                  config->dispHeight);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG6 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG6];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halCigMakeCigReg7Reg(
                  regOvly->CIG_REG7,
                  config->xPos,
                  config->yPos);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG7 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG7];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halCigMakeCigReg8Reg(
                  regOvly->CIG_REG8,
                  config->pipWidth,
                  config->pipHeight);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG8 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG8];
        *virRegOffset = tempReg;
    }

    return (retVal);
}



/**
 *  VpsHal_cigSetHdmiTransConfig
 *  \brief Sets the CIG HDMI transparency configuration to either the actual
 *  CIG registers or to the configuration overlay memory.
 */
Int32 VpsHal_cigSetHdmiTransConfig(VpsHal_Handle handle,
                                   const VpsHal_CigTransConfig *config,
                                   Ptr configOvlyPtr)
{
    Int32               retVal = VPS_SOK;
    VpsHal_CigObj      *cigObj;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Make register */
    tempReg = halCigMakeCigReg2Reg(
                  regOvly->CIG_REG2,
                  config->transparency,
                  config->mask,
                  config->alphaBlending,
                  config->alphaValue);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG2 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG2];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halCigMakeCigReg3Reg(
                  regOvly->CIG_REG3,
                  config->rTransColor,
                  config->gTransColor,
                  config->bTransColor);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG3 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG3];
        *virRegOffset = tempReg;
    }

    return (retVal);
}



/**
 *  VpsHal_cigSetHdcompTransConfig
 *  \brief Sets the CIG HDCOMP transparency configuration to either the actual
 *  CIG registers or to the configuration overlay memory.
 */
Int32 VpsHal_cigSetHdcompTransConfig(VpsHal_Handle handle,
                                     const VpsHal_CigTransConfig *config,
                                     Ptr configOvlyPtr)
{
    Int32               retVal = VPS_SOK;
    VpsHal_CigObj      *cigObj;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Make register */
    tempReg = halCigMakeCigReg4Reg(
                  regOvly->CIG_REG4,
                  config->transparency,
                  config->mask,
                  config->alphaBlending,
                  config->alphaValue);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG4 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG4];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halCigMakeCigReg5Reg(
                  regOvly->CIG_REG5,
                  config->rTransColor,
                  config->gTransColor,
                  config->bTransColor);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG5 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG5];
        *virRegOffset = tempReg;
    }

    return (retVal);
}



/**
 *  VpsHal_cigSetPipTransConfig
 *  \brief Sets the CIG PIP transparency configuration to either the actual
 *  CIG registers or to the configuration overlay memory.
 */
Int32 VpsHal_cigSetPipTransConfig(VpsHal_Handle handle,
                                  const VpsHal_CigTransConfig *config,
                                  Ptr configOvlyPtr)
{
    Int32               retVal = VPS_SOK;
    VpsHal_CigObj      *cigObj;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    cigObj = (VpsHal_CigObj *) handle;
    regOvly = cigObj->baseAddress;

    /* Make register */
    tempReg = halCigMakeCigReg9Reg(
                  regOvly->CIG_REG9,
                  config->transparency,
                  config->mask,
                  config->alphaBlending,
                  config->alphaValue);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG9 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG9];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halCigMakeCigReg10Reg(
                  regOvly->CIG_REG10,
                  config->rTransColor,
                  config->gTransColor,
                  config->bTransColor);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->CIG_REG10 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            cigObj->virRegOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG10];
        *virRegOffset = tempReg;
    }

    return (retVal);
}



/**
 *  VpsHal_cigGetPipConfig
 *  \brief Gets the CIG PIP configuration from the actual CIG registers.
 */
Int32 VpsHal_cigGetPipConfig(VpsHal_Handle handle,
                             VpsHal_CigPipConfig *config)
{
    Int32               retVal = VPS_SOK;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_CigObj *) handle)->baseAddress;

    /* Get PIP display width and height configurations */
    tempReg = regOvly->CIG_REG6;
    config->dispWidth = ((tempReg & CSL_VPS_CIG_CIG_REG6_PIP_DISP_W_MASK) >>
                            CSL_VPS_CIG_CIG_REG6_PIP_DISP_W_SHIFT);
    config->dispHeight = ((tempReg & CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_MASK) >>
                            CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_SHIFT);

    /* Get PIP x and y position configurations */
    tempReg = regOvly->CIG_REG7;
    config->xPos = ((tempReg & CSL_VPS_CIG_CIG_REG7_PIP_X_MASK) >>
                            CSL_VPS_CIG_CIG_REG7_PIP_X_SHIFT);
    config->yPos = ((tempReg & CSL_VPS_CIG_CIG_REG7_PIP_Y_MASK) >>
                            CSL_VPS_CIG_CIG_REG7_PIP_Y_SHIFT);

    /* Get PIP display width and height configurations */
    tempReg = regOvly->CIG_REG8;
    config->pipWidth = ((tempReg & CSL_VPS_CIG_CIG_REG8_PIP_W_MASK) >>
                            CSL_VPS_CIG_CIG_REG8_PIP_W_SHIFT);
    config->pipHeight = ((tempReg & CSL_VPS_CIG_CIG_REG8_PIP_H_MASK) >>
                            CSL_VPS_CIG_CIG_REG8_PIP_H_SHIFT);

    return (retVal);
}



/**
 *  VpsHal_cigGetHdmiTransConfig
 *  \brief Gets the CIG HDMI transparency configuration from the actual CIG
 *  registers.
 */
Int32 VpsHal_cigGetHdmiTransConfig(VpsHal_Handle handle,
                                   VpsHal_CigTransConfig *config)
{
    Int32               retVal = VPS_SOK;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32              transColor;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_CigObj *) handle)->baseAddress;

    /* Get transparency/blending configurations */
    tempReg = regOvly->CIG_REG2;

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG2_TR_ENABLE_MASK))
    {
        config->transparency = TRUE;
    }
    else            /* Transparency disabled */
    {
        config->transparency = FALSE;
    }

    config->mask = (VpsHal_CigTransMask) ((tempReg &
                       CSL_VPS_CIG_CIG_REG2_TR_MODE_MASK_MASK) >>
                       CSL_VPS_CIG_CIG_REG2_TR_MODE_MASK_SHIFT);

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG2_BL_ENABLE_MASK))
    {
        config->alphaBlending = TRUE;
    }
    else            /* Alpha Blending disabled */
    {
        config->alphaBlending = FALSE;
    }

    config->alphaValue = ((tempReg & CSL_VPS_CIG_CIG_REG2_BL_LEVEL_MASK) >>
                             CSL_VPS_CIG_CIG_REG2_BL_LEVEL_SHIFT);

    /* Get transparency color configuration */
    tempReg = regOvly->CIG_REG3;
    transColor = ((tempReg & CSL_VPS_CIG_CIG_REG3_TR_COLOR_MASK) >>
                             CSL_VPS_CIG_CIG_REG3_TR_COLOR_SHIFT);
    config->rTransColor = (UInt8) ((transColor >> 16u) & 0xFFu);
    config->gTransColor = (UInt8) ((transColor >> 8u) & 0xFFu);
    config->bTransColor = (UInt8) ((transColor >> 0u) & 0xFFu);

    return (retVal);
}



/**
 *  VpsHal_cigGetHdcompTransConfig
 *  \brief Gets the CIG HDCOMP transparency configuration from the actual CIG
 *  registers.
 */
Int32 VpsHal_cigGetHdcompTransConfig(VpsHal_Handle handle,
                                     VpsHal_CigTransConfig *config)
{
    Int32               retVal = VPS_SOK;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32              transColor;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_CigObj *) handle)->baseAddress;

    /* Get transparency/blending configurations */
    tempReg = regOvly->CIG_REG4;

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG4_TR_ENABLE_MASK))
    {
        config->transparency = TRUE;
    }
    else            /* Transparency disabled */
    {
        config->transparency = FALSE;
    }

    config->mask = (VpsHal_CigTransMask) ((tempReg &
                       CSL_VPS_CIG_CIG_REG4_TR_MODE_MASK_MASK) >>
                       CSL_VPS_CIG_CIG_REG4_TR_MODE_MASK_SHIFT);

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG4_BL_ENABLE_MASK))
    {
        config->alphaBlending = TRUE;
    }
    else            /* Alpha Blending disabled */
    {
        config->alphaBlending = FALSE;
    }

    config->alphaValue = ((tempReg & CSL_VPS_CIG_CIG_REG4_BL_LEVEL_MASK) >>
                             CSL_VPS_CIG_CIG_REG4_BL_LEVEL_SHIFT);

    /* Get transparency color configuration */
    tempReg = regOvly->CIG_REG5;
    transColor = ((tempReg & CSL_VPS_CIG_CIG_REG5_TR_COLOR_MASK) >>
                             CSL_VPS_CIG_CIG_REG5_TR_COLOR_SHIFT);
    config->rTransColor = (UInt8) ((transColor >> 16u) & 0xFFu);
    config->gTransColor = (UInt8) ((transColor >> 8u) & 0xFFu);
    config->bTransColor = (UInt8) ((transColor >> 0u) & 0xFFu);

    return (retVal);
}



/**
 *  VpsHal_cigGetPipTransConfig
 *  \brief Gets the CIG PIP transparency configuration from the actual CIG
 *  registers.
 */
Int32 VpsHal_cigGetPipTransConfig(VpsHal_Handle handle,
                                  VpsHal_CigTransConfig *config)
{
    Int32               retVal = VPS_SOK;
    UInt32              tempReg;
    CSL_VpsCigRegsOvly  regOvly;
    UInt32              transColor;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_CigObj *) handle)->baseAddress;

    /* Get transparency/blending configurations */
    tempReg = regOvly->CIG_REG9;

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG9_TR_ENABLE_MASK))
    {
        config->transparency = TRUE;
    }
    else            /* Transparency disabled */
    {
        config->transparency = FALSE;
    }

    config->mask = (VpsHal_CigTransMask) ((tempReg &
                       CSL_VPS_CIG_CIG_REG9_TR_MODE_MASK_MASK) >>
                       CSL_VPS_CIG_CIG_REG9_TR_MODE_MASK_SHIFT);

    if (0 != (tempReg & CSL_VPS_CIG_CIG_REG9_BL_ENABLE_MASK))
    {
        config->alphaBlending = TRUE;
    }
    else            /* Alpha Blending disabled */
    {
        config->alphaBlending = FALSE;
    }

    config->alphaValue = ((tempReg & CSL_VPS_CIG_CIG_REG9_BL_LEVEL_MASK) >>
                             CSL_VPS_CIG_CIG_REG9_BL_LEVEL_SHIFT);

    /* Get transparency color configuration */
    tempReg = regOvly->CIG_REG10;
    transColor = ((tempReg & CSL_VPS_CIG_CIG_REG10_TR_COLOR_MASK) >>
                             CSL_VPS_CIG_CIG_REG10_TR_COLOR_SHIFT);
    config->rTransColor = (UInt8) ((transColor >> 16u) & 0xFFu);
    config->gTransColor = (UInt8) ((transColor >> 8u) & 0xFFu);
    config->bTransColor = (UInt8) ((transColor >> 0u) & 0xFFu);

    return (retVal);
}



/**
 *  halCigInitInstance
 *  \brief Initialize the CIG instance by writing to expert registers and
 *  figuring out the virtual offsets of registers for config overlay memory.
 */
static Int32 halCigInitInstance(VpsHal_CigObj *cigObj)
{
    CSL_VpsCigRegsOvly      regOvly;

    regOvly = cigObj->baseAddress;

    /* Initialize the register offsets for the registers which are used
     * to form the VPDMA configuration overlay */
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG0] =
                                    (UInt32 *) &regOvly->CIG_REG0;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG1] =
                                    (UInt32 *) &regOvly->CIG_REG1;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG2] =
                                    (UInt32 *) &regOvly->CIG_REG2;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG3] =
                                    (UInt32 *) &regOvly->CIG_REG3;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG4] =
                                    (UInt32 *) &regOvly->CIG_REG4;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG5] =
                                    (UInt32 *) &regOvly->CIG_REG5;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG6] =
                                    (UInt32 *) &regOvly->CIG_REG6;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG7] =
                                    (UInt32 *) &regOvly->CIG_REG7;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG8] =
                                    (UInt32 *) &regOvly->CIG_REG8;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG9] =
                                    (UInt32 *) &regOvly->CIG_REG9;
    cigObj->regOffset[VPSHAL_CIG_CONFIGOVLYIDX_CIG_REG10] =
                                    (UInt32 *) &regOvly->CIG_REG10;

    /* Get the size of the overlay for CIG registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    cigObj->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                 cigObj->regOffset,
                                 VPSHAL_CIG_NUM_REG_IDX,
                                 cigObj->virRegOffset);

    /* Initialize the register offsets for the registers which are used
     * to form the runtime VPDMA configuration overlay */
    cigObj->rtRegOffset[VPSHAL_CIG_RT_CONFIGOVLYIDX_CIG_REG7] =
                                    (UInt32 *) &regOvly->CIG_REG7;
    cigObj->rtRegOffset[VPSHAL_CIG_RT_CONFIGOVLYIDX_CIG_REG8] =
                                    (UInt32 *) &regOvly->CIG_REG8;

    /* Get the size of the overlay for CIG registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    cigObj->rtConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                   cigObj->rtRegOffset,
                                   VPSHAL_CIG_NUM_RT_REG_IDX,
                                   cigObj->rtVirRegOffset);


    cigObj->pipEnableRegOffset[VPSHAL_CIG_PIP_ENABLE_CONFIGOVLYIDX_REG0] =
                        (UInt32 *) &regOvly->CIG_REG0;

    /* Get the size of the overlay for CIG registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    cigObj->pipEnableConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                   cigObj->pipEnableRegOffset,
                                   VPSHAL_CIG_NUM_PIP_ENABLE_REG_IDX,
                                   cigObj->pipEnableVirRegOffset);
    return 0;
}



/**
 *  halCigMakeCigReg0Reg
 *  \brief Make CIG REG0 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg0Reg(UInt32 initRegVal,
                                          UInt32 enableCig,
                                          UInt32 nonCtrInterlace,
                                          UInt32 ctrInterlace,
                                          UInt32 pipInterlace,
                                          UInt32 vertDecimation,
                                          UInt32 fldRepeat,
                                          UInt32 enablePip,
                                          VpsHal_CigPipWinSz pipWinSz)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG0_CIG_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_P2I_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_CI_P2I_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_PIP_P2I_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_CI_VDEC_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_CI_FIELD_RPT_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK |
                 CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_MASK);

    /* Set CIG enable */
    if (TRUE == enableCig)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_CIG_EN_MASK;
    }

    /* Set Progressive to Interlace conversion for non-constrained output */
    if (TRUE == nonCtrInterlace)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_P2I_EN_MASK;
    }

    /* Set Progressive to Interlace conversion for constrained output */
    if (TRUE == ctrInterlace)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_CI_P2I_EN_MASK;
    }

    /* Set Progressive to Interlace conversion for PIP output */
    if (TRUE == pipInterlace)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_PIP_P2I_EN_MASK;
    }

    /* Set vertical decimation */
    if (TRUE == vertDecimation)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_CI_VDEC_EN_MASK;
    }

    /* Set field repeat mode */
    if (TRUE == fldRepeat)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_CI_FIELD_RPT_EN_MASK;
    }

    /* Set PIP path enable */
    if (TRUE == enablePip)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK;
    }

    /* Set PIP window size */
    if (VPSHAL_CIG_PWS_FULL_SIZE == pipWinSz)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_MASK;
    }

    return tempReg;
}



/**
 *  halCigMakeCigReg1Reg
 *  \brief Make CIG REG1 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg1Reg(UInt32 initRegVal,
                                          UInt16 width,
                                          UInt16 height)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG1_DISP_W_MASK |
                 CSL_VPS_CIG_CIG_REG1_DISP_H_MASK);

    /* Set Width and Height */
    tempReg |= (((UInt32) width) << CSL_VPS_CIG_CIG_REG1_DISP_W_SHIFT);
    tempReg |= (((UInt32) height) << CSL_VPS_CIG_CIG_REG1_DISP_H_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg2Reg
 *  \brief Make CIG REG2 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg2Reg(UInt32 initRegVal,
                                          UInt32 transparency,
                                          VpsHal_CigTransMask mask,
                                          UInt32 alphaBlending,
                                          UInt8 alphaValue)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG2_TR_ENABLE_MASK |
                 CSL_VPS_CIG_CIG_REG2_TR_MODE_MASK_MASK |
                 CSL_VPS_CIG_CIG_REG2_BL_ENABLE_MASK |
                 CSL_VPS_CIG_CIG_REG2_BL_LEVEL_MASK);

    /* Set Transparency enable */
    if (TRUE == transparency)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG2_TR_ENABLE_MASK;
    }

    /* Set Transparency mask */
    tempReg |= (((UInt32) mask) << CSL_VPS_CIG_CIG_REG2_TR_MODE_MASK_SHIFT);

    /* Set Alpha Blending enable */
    if (TRUE == alphaBlending)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG2_BL_ENABLE_MASK;
    }

    /* Set Alpha Blending value */
    tempReg |= (((UInt32) alphaValue) << CSL_VPS_CIG_CIG_REG2_BL_LEVEL_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg3Reg
 *  \brief Make CIG REG3 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg3Reg(UInt32 initRegVal,
                                          UInt8 rTransColor,
                                          UInt8 gTransColor,
                                          UInt8 bTransColor)
{
    UInt32      tempReg;
    UInt32      transColor;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG3_TR_COLOR_MASK);

    /* Form the 24-bit RGB Transparency color */
    transColor = ((((UInt32) rTransColor) << 16u) |
                  (((UInt32) gTransColor) << 8u) |
                  (((UInt32) bTransColor) << 0u));

    /* Set 24-bit RGB Transparency color */
    tempReg |= (((UInt32) transColor) << CSL_VPS_CIG_CIG_REG3_TR_COLOR_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg4Reg
 *  \brief Make CIG REG4 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg4Reg(UInt32 initRegVal,
                                          UInt32 transparency,
                                          VpsHal_CigTransMask mask,
                                          UInt32 alphaBlending,
                                          UInt8 alphaValue)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG4_TR_ENABLE_MASK |
                 CSL_VPS_CIG_CIG_REG4_TR_MODE_MASK_MASK |
                 CSL_VPS_CIG_CIG_REG4_BL_ENABLE_MASK |
                 CSL_VPS_CIG_CIG_REG4_BL_LEVEL_MASK);

    /* Set Transparency enable */
    if (TRUE == transparency)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG4_TR_ENABLE_MASK;
    }

    /* Set Transparency mask */
    tempReg |= (((UInt32) mask) << CSL_VPS_CIG_CIG_REG4_TR_MODE_MASK_SHIFT);

    /* Set Alpha Blending enable */
    if (TRUE == alphaBlending)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG4_BL_ENABLE_MASK;
    }

    /* Set Alpha Blending value */
    tempReg |= (((UInt32) alphaValue) << CSL_VPS_CIG_CIG_REG4_BL_LEVEL_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg5Reg
 *  \brief Make CIG REG5 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg5Reg(UInt32 initRegVal,
                                          UInt8 rTransColor,
                                          UInt8 gTransColor,
                                          UInt8 bTransColor)
{
    UInt32      tempReg;
    UInt32      transColor;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG5_TR_COLOR_MASK);

    /* Form the 24-bit RGB Transparency color */
    transColor = ((((UInt32) rTransColor) << 16u) |
                  (((UInt32) gTransColor) << 8u) |
                  (((UInt32) bTransColor) << 0u));

    /* Set 24-bit RGB Transparency color */
    tempReg |= (((UInt32) transColor) << CSL_VPS_CIG_CIG_REG5_TR_COLOR_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg6Reg
 *  \brief Make CIG REG6 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg6Reg(UInt32 initRegVal,
                                          UInt16 dispWidth,
                                          UInt16 dispHeight)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG6_PIP_DISP_W_MASK |
                 CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_MASK);

    /* Set PIP Width and Height */
    tempReg |= (((UInt32) dispWidth) << CSL_VPS_CIG_CIG_REG6_PIP_DISP_W_SHIFT);
    tempReg |= (((UInt32) dispHeight) << CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg7Reg
 *  \brief Make CIG REG7 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg7Reg(UInt32 initRegVal,
                                          UInt16 xPos,
                                          UInt16 yPos)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG7_PIP_X_MASK |
                 CSL_VPS_CIG_CIG_REG7_PIP_Y_MASK);

    /* Set PIP X position and Y position */
    tempReg |= (((UInt32) xPos) << CSL_VPS_CIG_CIG_REG7_PIP_X_SHIFT);
    tempReg |= (((UInt32) yPos) << CSL_VPS_CIG_CIG_REG7_PIP_Y_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg8Reg
 *  \brief Make CIG REG8 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg8Reg(UInt32 initRegVal,
                                          UInt16 pipWidth,
                                          UInt16 pipHeight)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG8_PIP_W_MASK |
                 CSL_VPS_CIG_CIG_REG8_PIP_H_MASK);

    /* Set PIP window Width and Height */
    tempReg |= (((UInt32) pipWidth) << CSL_VPS_CIG_CIG_REG8_PIP_W_SHIFT);
    tempReg |= (((UInt32) pipHeight) << CSL_VPS_CIG_CIG_REG8_PIP_H_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg9Reg
 *  \brief Make CIG REG9 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg9Reg(UInt32 initRegVal,
                                          UInt32 transparency,
                                          VpsHal_CigTransMask mask,
                                          UInt32 alphaBlending,
                                          UInt8 alphaValue)
{
    UInt32      tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG9_TR_ENABLE_MASK |
                 CSL_VPS_CIG_CIG_REG9_TR_MODE_MASK_MASK |
                 CSL_VPS_CIG_CIG_REG9_BL_ENABLE_MASK |
                 CSL_VPS_CIG_CIG_REG9_BL_LEVEL_MASK);

    /* Set Transparency enable */
    if (TRUE == transparency)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG9_TR_ENABLE_MASK;
    }

    /* Set Transparency mask */
    tempReg |= (((UInt32) mask) << CSL_VPS_CIG_CIG_REG9_TR_MODE_MASK_SHIFT);

    /* Set Alpha Blending enable */
    if (TRUE == alphaBlending)
    {
        tempReg |= CSL_VPS_CIG_CIG_REG9_BL_ENABLE_MASK;
    }

    /* Set Alpha Blending value */
    tempReg |= (((UInt32) alphaValue) << CSL_VPS_CIG_CIG_REG9_BL_LEVEL_SHIFT);

    return tempReg;
}



/**
 *  halCigMakeCigReg10Reg
 *  \brief Make CIG REG10 register with given parameters.
 */
static inline UInt32 halCigMakeCigReg10Reg(UInt32 initRegVal,
                                           UInt8 rTransColor,
                                           UInt8 gTransColor,
                                           UInt8 bTransColor)
{
    UInt32      tempReg;
    UInt32      transColor;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_CIG_CIG_REG10_TR_COLOR_MASK);

    /* Form the 24-bit RGB Transparency color */
    transColor = ((((UInt32) rTransColor) << 16u) |
                  (((UInt32) gTransColor) << 8u) |
                  (((UInt32) bTransColor) << 0u));

    /* Set 24-bit RGB Transparency color */
    tempReg |= (((UInt32) transColor) << CSL_VPS_CIG_CIG_REG10_TR_COLOR_SHIFT);

    return tempReg;
}
