/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_csc.c
 *
 * \brief VPS Color Space Converter Source file.
 * This file implements the HAL APIs of the VPS Color Space Converter.
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
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/cslr/cslr_vps_csc.h>
#include <ti/psp/vps/hal/src/vpshalCscDefaults.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* TODO: Is it Required? */
/**
 *  enum Csc_ConfigOvlyReg
 *  \brief Enums for the configuration overlay register offsets - used as array
 *  indices to register offset array in Csc_Obj object. These enums defines the
 *  registers that will be used to form the configuration register overlay for
 *  CSC module. This in turn is used by client drivers to configure registers
 *  using VPDMA during context-switching (software channels with different
 *  configurations).
 *
 *  Note: If in future it is decided to remove or add some register, this enum
 *  should be modified accordingly. And make sure that the values of these
 *  enums are in ascending order and it starts from 0.
 */
typedef enum
{
    CSC_CONFIG_OVLY_CSC00_IDX = 0,
    CSC_CONFIG_OVLY_CSC01_IDX,
    CSC_CONFIG_OVLY_CSC02_IDX,
    CSC_CONFIG_OVLY_CSC03_IDX,
    CSC_CONFIG_OVLY_CSC04_IDX,
    CSC_CONFIG_OVLY_CSC05_IDX,
    CSC_CONFIG_OVLY_NUM_IDX
    /**< This should be the last enum */
} Csc_ConfigOvlyReg;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief struct Csc_Info
 *  Structure containing Instance Information for each CSC module
 *
 *  parent - Pointer to the main scalar info structure
 */
typedef struct
{
    UInt32              instId;
    /**< Indicates the path on which CSC is available */
    UInt32              openCnt;
    /**< Keeps track of number of open for a Csc instance */
    CSL_VpsCscRegsOvly  regOvly;
    /**< Pointer to the CSLR register overlay structure */
    UInt32              configOvlySize;
    /**< Virtual register overlay size in terms of bytes. Used to allocate
         memory for virtual register configured through VPDMA */
    UInt32             *regOffset[CSC_CONFIG_OVLY_NUM_IDX];
    /**< Array of physical address of the register configured through VPDMA
         register overlay. */
    UInt32              virtRegOffset[CSC_CONFIG_OVLY_NUM_IDX];
    /**< Array of indexes into the Overlay memory where virtual
         registers are to be configured */
} Csc_Info;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static inline UInt32 cscMakeReg0(UInt16 coeff1,
                                 UInt16 coeff2);
static inline UInt32 cscMakeReg1(UInt16 coeff1,
                                 UInt16 coeff2);
static inline UInt32 cscMakeReg2(UInt16 coeff1,
                                 UInt16 coeff2);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief CSC objects - Module variable to store information about each CSC
 *  instance.
 *  Note: If the number of CSC instance increase, then this variable should be
 *  initialized with the added CSC instance information.
 */
static Csc_Info CscObj[VPSHAL_CSC_MAX_INST];

/**
 *  \brief Default Value of Color Space Conversion coefficients
 */
static Vps_CscCoeff CscDefaultCoeff[VPS_CSC_MODE_MAX] =
                                VPSHAL_CSC_DEFAULT_EXPERT_VAL;

/* ========================================================================== */
/*                          Function Definitions                              */
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
                   Ptr arg)
{
    Int                 instCnt;
    CSL_VpsCscRegsOvly  regOvly = NULL;
    Int                 ret = 0;
    Csc_Info           *instInfo = NULL;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_CSC_MAX_INST));
    GT_assert(VpsHalTrace, (NULL != initParams));

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < numInstances; instCnt ++)
    {
        GT_assert(VpsHalTrace,
                    (initParams[instCnt].instId < VPSHAL_CSC_MAX_INST));

        /* Initialize Instance Specific Parameters */
        instInfo = &(CscObj[instCnt]);
        instInfo->openCnt = 0u;
        instInfo->regOvly =
                (CSL_VpsCscRegsOvly) initParams[instCnt].baseAddress;
        instInfo->instId = initParams[instCnt].instId;

        /* Calculate VPDMA register overlay size and register offset in
         * the VPDMA register overlay. Since register organization for all
         * CSC are same, only one array of register offset is sufficient. */
        /* Since the register offset are same for all CSC instances, using HQ
         * CSC to get physical offset. */
        regOvly = instInfo->regOvly;
        instInfo->regOffset[CSC_CONFIG_OVLY_CSC00_IDX] =
                                                (UInt32 *)&(regOvly->CSC00);
        instInfo->regOffset[CSC_CONFIG_OVLY_CSC01_IDX] =
                                                (UInt32 *)&(regOvly->CSC01);
        instInfo->regOffset[CSC_CONFIG_OVLY_CSC02_IDX] =
                                                (UInt32 *)&(regOvly->CSC02);
        instInfo->regOffset[CSC_CONFIG_OVLY_CSC03_IDX] =
                                                (UInt32 *)&(regOvly->CSC03);
        instInfo->regOffset[CSC_CONFIG_OVLY_CSC04_IDX] =
                                                (UInt32 *)&(regOvly->CSC04);
        instInfo->regOffset[CSC_CONFIG_OVLY_CSC05_IDX] =
                                                (UInt32 *)&(regOvly->CSC05);

        instInfo->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                    instInfo->regOffset,
                                    CSC_CONFIG_OVLY_NUM_IDX,
                                    instInfo->virtRegOffset);
    }

    return (ret);
}



/**
 *  VpsHal_cscDeInit
 *  \brief CSC HAL exit function.
 *  Currently this function does not do anything.
 *
 *  \param arg     Not used currently. Meant for future purpose.
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_cscDeInit(Ptr arg)
{
    return (0);
}



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
VpsHal_Handle VpsHal_cscOpen(UInt32 cscInst)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;
    Csc_Info           *instInfo = NULL;

    for (cnt = 0; cnt < VPSHAL_CSC_MAX_INST; cnt++)
    {
        instInfo = &(CscObj[cnt]);
        /* Return the matching instance handle */
        if (cscInst == instInfo->instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            handle = (VpsHal_Handle) (instInfo);

            /* Check whether some one has already opened this instance */
            if (0 == instInfo->openCnt)
            {
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
Int VpsHal_cscClose(VpsHal_Handle handle)
{
    Int            ret = -1;
    UInt32         cookie;
    Csc_Info      *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    instInfo = (Csc_Info *) handle;

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



/* TODO: To Decide whether to configuration through VPDMA is required */
/* Returns the size (in bytes) of the VPDMA register configuration overlay
 * required to program the CSC registers using VPDMA config descriptors */
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
UInt32 VpsHal_cscGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32 configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((Csc_Info *) handle)->configOvlySize;

    return (configOvlySize);
}



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
Int VpsHal_cscCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int ret;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    ret = VpsHal_vpdmaCreateRegOverlay(
              ((Csc_Info *) handle)->regOffset,
              CSC_CONFIG_OVLY_NUM_IDX,
              configOvlyPtr);

    return (ret);
}



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
                        Ptr configOvlyPtr)
{
    Int                 ret = 0;
    CSL_VpsCscRegsOvly  regOvly = NULL;
    Vps_CscCoeff    *coeff = NULL;
    UInt32              tempReg;
    Csc_Info           *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Get the register Overlay pointer */
    instInfo = (Csc_Info *) handle;
    regOvly = instInfo->regOvly;

    /* Get the coefficient pointer either from config or from default coeff */
    if (VPS_CSC_MODE_NONE != config->mode)
    {
        coeff = &(CscDefaultCoeff[(UInt32)config->mode]);
    }
    else
    {
        coeff = config->coeff;
    }

    /* Configure CSC if it is not in bypass mode */
    if (FALSE == config->bypass)
    {
        /* Check whether parameters are valid */
        GT_assert (VpsHalTrace,
            (((NULL != config->coeff) && (VPS_CSC_MODE_NONE == config->mode)) ||
             ((NULL == config->coeff) && (VPS_CSC_MODE_NONE != config->mode))));

        if (NULL == configOvlyPtr)
        {
            /* Set the coefficients in the actual register */
            regOvly->CSC00 = cscMakeReg0(coeff->mulCoeff[0][0],
                                         coeff->mulCoeff[0][1]);
            regOvly->CSC01 = cscMakeReg0(coeff->mulCoeff[0][2],
                                         coeff->mulCoeff[1][0]);
            regOvly->CSC02 = cscMakeReg0(coeff->mulCoeff[1][1],
                                         coeff->mulCoeff[1][2]);
            regOvly->CSC03 = cscMakeReg0(coeff->mulCoeff[2][0],
                                         coeff->mulCoeff[2][1]);
            regOvly->CSC04 = cscMakeReg1(coeff->mulCoeff[2][2],
                                         coeff->addCoeff[0]);
            regOvly->CSC05 = cscMakeReg2(coeff->addCoeff[1],
                                         coeff->addCoeff[2]);
            }
        else
        {
            /* Set the coefficients in Virtual Register Overlay */
            tempReg = cscMakeReg0(
                        coeff->mulCoeff[0][0],
                        coeff->mulCoeff[0][1]);
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC00_IDX]) = tempReg;

            tempReg = cscMakeReg0(
                        coeff->mulCoeff[0][2],
                        coeff->mulCoeff[1][0]);
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC01_IDX]) = tempReg;

            tempReg = cscMakeReg0(
                        coeff->mulCoeff[1][1],
                        coeff->mulCoeff[1][2]);
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC02_IDX]) = tempReg;

            tempReg = cscMakeReg0(
                        coeff->mulCoeff[2][0],
                        coeff->mulCoeff[2][1]);
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC03_IDX]) = tempReg;

            tempReg = cscMakeReg1(
                        coeff->mulCoeff[2][2],
                        coeff->addCoeff[0]);
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC04_IDX]) = tempReg;

            tempReg = cscMakeReg2(
                        coeff->addCoeff[1],
                        coeff->addCoeff[2]);
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC05_IDX]) = tempReg;
        }
    }
    else
    {
        /* CSC is in bypass mode so enable bypass in CSC */
        if (NULL == configOvlyPtr)
        {
            regOvly->CSC05 = CSL_VPS_CSC_CSC05_BYPASS_MASK;
        }
        else
        {
            *((UInt32 *)configOvlyPtr +
                instInfo->virtRegOffset[CSC_CONFIG_OVLY_CSC05_IDX]) =
                CSL_VPS_CSC_CSC05_BYPASS_MASK;
        }
    }

    return (ret);
}



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
Int VpsHal_cscGetConfig(VpsHal_Handle handle, Vps_CscConfig *config)
{
    CSL_VpsCscRegsOvly         regOvly = NULL;
    Vps_CscCoeff           *coeff = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));
    GT_assert(VpsHalTrace, (NULL != config->coeff));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = ((Csc_Info *) handle)->regOvly;
    coeff = config->coeff;

    if (0 != (regOvly->CSC05 &
                   CSL_VPS_CSC_CSC05_BYPASS_MASK) >>
                   CSL_VPS_CSC_CSC05_BYPASS_SHIFT)
    {
        config->bypass = TRUE;
    }
    else
    {
        config->bypass = FALSE;
    }
    if (FALSE == config->bypass)
    {
        /* Get the coefficients from the registers */
        coeff->mulCoeff[0][0] = (regOvly->CSC00 & CSL_VPS_CSC_CSC00_A0_MASK) >>
                    CSL_VPS_CSC_CSC00_A0_SHIFT;
        coeff->mulCoeff[0][1] = (regOvly->CSC00 & CSL_VPS_CSC_CSC00_B0_MASK) >>
                    CSL_VPS_CSC_CSC00_B0_SHIFT;
        coeff->mulCoeff[0][2] = (regOvly->CSC01 & CSL_VPS_CSC_CSC01_C0_MASK) >>
                    CSL_VPS_CSC_CSC01_C0_SHIFT;
        coeff->mulCoeff[1][0] = (regOvly->CSC01 & CSL_VPS_CSC_CSC01_A1_MASK) >>
                    CSL_VPS_CSC_CSC01_A1_SHIFT;
        coeff->mulCoeff[1][1] = (regOvly->CSC02 & CSL_VPS_CSC_CSC02_B1_MASK) >>
                    CSL_VPS_CSC_CSC02_B1_SHIFT;
        coeff->mulCoeff[1][2] = (regOvly->CSC02 & CSL_VPS_CSC_CSC02_C1_MASK) >>
                    CSL_VPS_CSC_CSC02_C1_SHIFT;
        coeff->mulCoeff[0][1] = (regOvly->CSC03 & CSL_VPS_CSC_CSC03_A2_MASK) >>
                    CSL_VPS_CSC_CSC03_A2_SHIFT;
        coeff->mulCoeff[0][1] = (regOvly->CSC03 & CSL_VPS_CSC_CSC03_B2_MASK) >>
                    CSL_VPS_CSC_CSC03_B2_SHIFT;
        coeff->mulCoeff[0][1] = (regOvly->CSC04 & CSL_VPS_CSC_CSC04_C2_MASK) >>
                    CSL_VPS_CSC_CSC04_C2_SHIFT;
        coeff->addCoeff[0] = (regOvly->CSC04 & CSL_VPS_CSC_CSC04_D0_MASK) >>
                    CSL_VPS_CSC_CSC04_D0_SHIFT;
        coeff->addCoeff[1] = (regOvly->CSC05 & CSL_VPS_CSC_CSC05_D1_MASK) >>
                    CSL_VPS_CSC_CSC05_D1_SHIFT;
        coeff->addCoeff[2] = (regOvly->CSC05 & CSL_VPS_CSC_CSC05_D2_MASK) >>
                    CSL_VPS_CSC_CSC05_D2_SHIFT;
    }

    return (0);
}



/**
 * cscMakeReg0
 * \brief Make the register of CSC for the coefficients which has same masks
 * and offsets.
 */
static inline UInt32 cscMakeReg0(UInt16 coeff1,
                                 UInt16 coeff2)
{
    UInt32 tempReg;

    /* Since all coefficients masks are same, onle masks for A0 and
     * B0 are used */
    tempReg = (coeff1 << CSL_VPS_CSC_CSC00_A0_SHIFT) &
                        CSL_VPS_CSC_CSC00_A0_MASK;
    tempReg |= (coeff2 << CSL_VPS_CSC_CSC00_B0_SHIFT) &
                        CSL_VPS_CSC_CSC00_B0_MASK;
    return (tempReg);
    }



/**
 * cscMakeReg1
 * \brief Make the register of CSC for the coefficients with coeff2 is of type
 * D0 and coeff1 is of type A0 so using different masks and offsets for both.
 */
static inline UInt32 cscMakeReg1(UInt16 coeff1,
                                 UInt16 coeff2)
    {
    UInt32 tempReg;

    /* Since all coefficients masks are same, onle masks for A0 and
     * D0 are used */
    tempReg = (coeff1 << CSL_VPS_CSC_CSC00_A0_SHIFT) &
                        CSL_VPS_CSC_CSC00_A0_MASK;
    tempReg |= (coeff2 << CSL_VPS_CSC_CSC04_D0_SHIFT) &
                        CSL_VPS_CSC_CSC04_D0_MASK;
    return (tempReg);
    }



/**
 *  cscMakeReg2
 *  \brief Make the register of CSC for the coefficients with both the
 *  coefficients are of type D0 so using masks of D1 and D2.
 */
static inline UInt32 cscMakeReg2(UInt16 coeff1,
                                 UInt16 coeff2)
{
    UInt32 tempReg;

    tempReg = (coeff1 << CSL_VPS_CSC_CSC05_D1_SHIFT) &
                        CSL_VPS_CSC_CSC05_D1_MASK;
    tempReg |= (coeff2 << CSL_VPS_CSC_CSC05_D2_SHIFT) &
                        CSL_VPS_CSC_CSC05_D2_MASK;
    return (tempReg);
}
