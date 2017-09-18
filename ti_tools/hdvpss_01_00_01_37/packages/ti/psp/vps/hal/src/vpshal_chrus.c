/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_chrus.c
 *
 * \brief VPS Chroma Up Sampler HAL Source file.
 * This file implements the HAL APIs of the VPS Chroma Up Sampler (420 -> 422).
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_chrus.h>
#include <ti/psp/cslr/cslr_vps_chr_us.h>
#include <ti/psp/vps/hal/src/vpshalChrusDefaults.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*!
 *  \brief Enums for the configuration overlay register offsets - used as array
 *  indices to register offset array. These enums defines the registers
 *  that will be used to form the configuration register overlay.
 *  This in turn is used by client drivers to configure registers using
 *  VPDMA during context-switching (software channels with different
 *  configurations).
 *
 *  Note: If in future it is decided to remove or add some register, this
 *  enum and the initial value of NUM_REG_IDX macro should be modified
 *  accordingly. And make sure that the values of these enums are in
 *  ascending order and it starts from 0.
 */
typedef enum
{
    CHRUS_CONFIGOVLYIDX_REG0 = 0,
    CHRUS_CONFIGOVLYIDX_REG1,
    CHRUS_CONFIGOVLYIDX_REG2,
    CHRUS_CONFIGOVLYIDX_REG3,
    CHRUS_CONFIGOVLYIDX_REG4,
    CHRUS_CONFIGOVLYIDX_REG5,
    CHRUS_CONFIGOVLYIDX_REG6,
    CHRUS_CONFIGOVLYIDX_REG7,
    CHRUS_CONFIGOVLYIDX_MAX
} Chrus_ConfigOvlyIdx;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure containing Instance Information for each CHRUS module
 *  parent -            Pointer to the main scalar info structure
 */
typedef struct
{
    UInt32                instId;
    /**< Indicates the path on which CHRUS is available */
    UInt32                openCnt;
    /**< Keeps track of number of open for a Chrus instance */
    CSL_VpsChrusRegsOvly  regOvly;
    /**< Pointer to the CSLR register overlay structure */
    UInt32                configOvlySize;
    /**< Virtual register overlay size in terms of bytes. Used to allocate
         memory for virtual register configured through VPDMA */
    UInt32               *regOffset[CHRUS_CONFIGOVLYIDX_MAX];
    /**< Array of physical address of the register configured through
         VPDMA register overlay. */
    UInt32                virRegOffset[CHRUS_CONFIGOVLYIDX_MAX];
    /**< Array of indexes into the Overlay memory where virtual registers
         are to be configured */
} ChrusInfo;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Function to make CHR_US register */
static inline UInt32 chrusMakeReg(UInt16 coeff1, UInt16 coeff2);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 * VpsHal_ChrusCoeff
 */
static VpsHal_ChrusCoeff ChrusDefaultCoeff[2] = VPSHAL_CHRUS_DEFAULT_EXPERT_VAL;

/**
 * \brief CHRUS objects - Module variable to store information about each CHRUS
 * instance.
 */
static ChrusInfo ChrusObj[VPSHAL_CHRUS_MAX_INST];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_chrusInit
 *  \brief Initializes CHRUS objects, gets the register overlay offsets for
 *  CHRUS registers.
 *  This function should be called before calling any of CHRUS HAL API's.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              Not used currently. Meant for future purpose.
 */
Int VpsHal_chrusInit(UInt32 numInstances,
                     const VpsHal_ChrusInstParams *initParams,
                     Ptr arg)
{
    Int                     instCnt;
    CSL_VpsChrusRegsOvly    regOvly = NULL;
    Int                     ret = 0;
    ChrusInfo              *instInfo = NULL;
    UInt32                  regCnt;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_CHRUS_MAX_INST));
    GT_assert(VpsHalTrace, (NULL != initParams));

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < numInstances; instCnt ++)
    {
        GT_assert(VpsHalTrace,
                    (initParams[instCnt].instId < VPSHAL_CHRUS_MAX_INST));

        /* Initialize Instance Specific Parameters */
        instInfo = &(ChrusObj[instCnt]);
        instInfo->openCnt = 0u;
        instInfo->regOvly =
                (CSL_VpsChrusRegsOvly ) initParams[instCnt].baseAddress;
        instInfo->instId = initParams[instCnt].instId;

        /* Calculate VPDMA register overlay size and register offset in
         * the VPDMA register overlay. Since register organization for all
         * CHRUS are same, only one array of register offset is sufficient. */
        /* Since the register offset are same for all CHRUS instances,
         * using one CHRUS to get physical offset. */
        regOvly = instInfo->regOvly;

        for (regCnt = 0u; regCnt < CHRUS_CONFIGOVLYIDX_MAX; regCnt ++)
        {
            instInfo->regOffset[regCnt] = (UInt32 *)&(regOvly->REG[regCnt]);
        }

        instInfo->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                    instInfo->regOffset,
                                    CHRUS_CONFIGOVLYIDX_MAX,
                                    instInfo->virRegOffset);
    }

    return (ret);
}



/**
 *  VpsHal_chrusDeInit
 *  \brief Currently this function does not do anything.
 *
 *  \param arg       Not used currently. Meant for future purpose.
 */
Int VpsHal_chrusDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_chrusOpen
 *  \brief This function should be called prior to calling any of the CHRUS HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param chrusInst       Requested CHRUS instance
 */
VpsHal_Handle VpsHal_chrusOpen(UInt32 chrusInst)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;
    ChrusInfo          *instInfo = NULL;

    for (cnt = 0; cnt < VPSHAL_CHRUS_MAX_INST; cnt++)
    {
        instInfo = &(ChrusObj[cnt]);
        /* Return the matching instance handle */
        if (chrusInst == instInfo->instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0 == instInfo->openCnt)
            {
                handle = (VpsHal_Handle) (instInfo);
                instInfo->openCnt ++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }

    return (handle);
}



/**
 *  VpsHal_chrusClose
 *  \brief This functions closes the chrus handle and after call to this
 *  function, chrus handle is not valid. It also decrements the reference
 *  counter.
 *
 *  \param handle       Requested CHRUS instance
 */
Int VpsHal_chrusClose(VpsHal_Handle handle)
{
    Int                 ret = -1;
    UInt32              cookie;
    ChrusInfo          *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    instInfo = (ChrusInfo *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (instInfo->openCnt > 0)
    {
        instInfo->openCnt--;
        ret = 0;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (ret);
}



/**
 *  VpsHal_chrusGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the CHRUS registers using VPDMA config
 *  descriptors.
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the CHRUS registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context
 *  (multiple instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay
 *  one-time and submit this to VPDMA everytime when context changes.
 *  Thus saving valuable CPU in programming the VPS registers.
 *
 *  \param  handle       Instance handle
 *  \return              Returns the size of config overlay memory
 */
UInt32 VpsHal_chrusGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32          configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((ChrusInfo *) handle)->configOvlySize;

    return configOvlySize;
}



/**
 *  VpsHal_chrusCreateConfigOvly
 *  \brief Creates the CHRUS register configuration overlay using VPDMA
 *  helper function.
 *
 *  This function does not initialize the overlay with CHRUS configuration.
 *  It is the responsibility of the client driver to configure the overlay
 *  by calling HalChrus_setConfig function before submitting the same
 *  to VPDMA for register configuration at runtime.
 *
 *  \param handle           Instance handle
 *  \param configOvlyPtr    Pointer to the overlay memory where the
 *                          overlay is formed. The memory for the overlay
 *                          should be allocated by client driver.
 *                          This parameter should be non-NULL.
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_chrusCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int32           retVal;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    retVal = VpsHal_vpdmaCreateRegOverlay(
                 ((ChrusInfo *) handle)->regOffset,
                 CHRUS_CONFIGOVLYIDX_MAX,
                 configOvlyPtr);

    return (retVal);
}



/**
 *  \brief Sets the entire CHRUS configuration to either the actual CHRUS
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire CHRUS registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual CHRUS MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in CHRUS using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_chrusConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          CHRUS registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_chrusSetConfig(VpsHal_Handle handle,
                          const VpsHal_ChrusConfig *config,
                          Ptr configOvlyPtr)
{
    Int                     ret = 0;
    ChrusInfo              *chrusObj = NULL;
    CSL_VpsChrusRegsOvly    regOvly;
    VpsHal_ChrusCoeff      *coeff = NULL;
    UInt32                  mode = 0, tempReg = 0, regCnt, index, coeffCnt;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    chrusObj = (ChrusInfo *) handle;
    regOvly = chrusObj->regOvly;

    if(VPS_SF_INTERLACED == config->mode)
    {
        mode = 1;
    }
    else
    {
        mode = 0;
    }
    if(NULL == config->coeff)
    {
        coeff = &(ChrusDefaultCoeff[mode]);
    }
    else
    {
        coeff = config->coeff;
    }
    mode = (config->cfgMode << CSL_VPS_CHR_US_REG0_CFG_MODE_SHIFT) &
                CSL_VPS_CHR_US_REG0_CFG_MODE_MASK;
    tempReg = mode;
    coeffCnt = index = 0;
    for (regCnt = 0; regCnt < CHRUS_CONFIGOVLYIDX_MAX; regCnt ++)
    {
        /* Increment the coefficient counter if 4 coefficients
         * are stored */
        coeffCnt = regCnt >> 1u;

        if (0 == (index & (VPSHAL_CHRUS_NUM_COEFFS - 1u)))
        {
            index = 0u;
        }
        /* All register format are same so using same function to
         * make the register */
        tempReg |= chrusMakeReg(
                    coeff->coeff[coeffCnt][index],
                    coeff->coeff[coeffCnt][index + 1u]);
        if (NULL != configOvlyPtr)
        {
            *((UInt32 *)configOvlyPtr +
                    chrusObj->virRegOffset[regCnt]) = tempReg;
        }
        else
        {
            regOvly->REG[regCnt] = tempReg;
        }

        tempReg = 0u;
        /* One register stores 2 coefficients, so increment index
         * by 2*/
        index += 2u;
    }

    return (ret);
}



/**
 *  VpsHal_chrusGetConfig
 *  \brief Gets the entire CHRUS configuration from the actual CHRUS registers.
 *
 *  This function gets the entire CHRUS configuration. This function
 *  gets the configuration of all the sub-modules in CHRUS using other
 *  get configuration function calls.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_chrusConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_chrusGetConfig(VpsHal_Handle handle, VpsHal_ChrusConfig *config)
{
    CSL_VpsChrusRegsOvly     regOvly;
    UInt32                   regCnt, coeffCnt, index;
    ChrusInfo               *chrusObj = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));
    GT_assert(VpsHalTrace, (NULL != config->coeff));

    /* Get the Register Overlay Pointer from the handle */
    chrusObj = (ChrusInfo *) handle;
    regOvly = chrusObj->regOvly;

    /* Get the Configuration Mode from the Register 0 */
    config->cfgMode = (VpsHal_ChrusCfgMode)( (regOvly->REG[0u] &
                                        CSL_VPS_CHR_US_REG0_CFG_MODE_MASK) >>
                                        CSL_VPS_CHR_US_REG0_CFG_MODE_SHIFT);
    coeffCnt = index = 0;
    for (regCnt = 0; regCnt < CHRUS_CONFIGOVLYIDX_MAX; regCnt ++)
    {
        /* Increment the coefficient counter if 4 coefficients
         * are stored */
        coeffCnt = regCnt >> 1u;

        if (0 == (index & (VPSHAL_CHRUS_NUM_COEFFS - 1u)))
        {
            index = 0u;
        }
        /* All register format are same so using same function to
         * make the register */
        config->coeff->coeff[coeffCnt][index] = (regOvly->REG[regCnt] &
                                CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C0_MASK) >>
                                CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C0_SHIFT;
        config->coeff->coeff[coeffCnt][index + 1u] = (regOvly->REG[regCnt] &
                                CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C1_MASK) >>
                                CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C1_SHIFT;
        /* One register stores 2 coefficients, so increment index
         * by 2*/
        index += 2u;
    }

    return (0);
}



/**
 * chrusMakeReg
 * \brief Makes the CHRUS register given two coefficient to be stored in the
 * register and returns the register value.
 */
static inline UInt32 chrusMakeReg(UInt16 coeff1, UInt16 coeff2)
{
    UInt32 tempReg;

    /* All coefficients width and position are same so this function uses
     * macros from reg0 */
    tempReg = (coeff1 << CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C0_SHIFT) &
                    CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C0_MASK;
    tempReg |= (coeff2 << CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C1_SHIFT) &
                    CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C1_MASK;

    return (tempReg);
}

