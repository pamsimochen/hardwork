/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_ede.c
 *
 *  \brief VPS EDE HAL Source file.
 *  This file implements the HAL APIs of the VPS Edge Enhancer.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/hal/vpshal_ede.h>
#include <ti/psp/cslr/cslr_vps_ede.h>
#include <ti/psp/vps/hal/src/vpshalEdeDefaults.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* TODO: Is it Required? */
/**
 *  enum Ede_ConfigOvlyRe
 *  \brief Enums for the configuration overlay register offsets - used as array indices
 *  to register offset array in Ede_Obj object. These enums defines the
 *  registers that will be used to form the configuration register overlay for
 *  EDE module. This in turn is used by client drivers to configure registers
 *  using VPDMA during context-switching (software channels with different
 *  configurations).
 *
 *  Note: If in future it is decided to remove or add some register, this enum
 *  should be modified accordingly. And make sure that the values of these
 *  enums are in ascending order and it starts from 0.
 */
typedef enum
{
    EDE_CONFIG_OVLY_CFG_EDE1_IDX = 0,
    EDE_CONFIG_OVLY_CFG_EDE3_IDX,
    EDE_CONFIG_OVLY_CFG_EDE4_IDX,
    EDE_CONFIG_OVLY_CFG_EDE5_IDX,
    EDE_CONFIG_OVLY_CFG_EDE16_IDX,
    EDE_CONFIG_OVLY_CFG_EDE17_IDX,
    EDE_CONFIG_OVLY_CFG_EDE18_IDX,
    EDE_CONFIG_OVLY_NUM_IDX
    /**< This should be the last enum */
} Ede_ConfigOvlyReg;

typedef enum
{
    VPSHAL_EDE_PEAKING_COEFF_0_6 = 0,
    VPSHAL_EDE_PEAKING_COEFF_0_8,
    VPSHAL_EDE_PEAKING_COEFF_1,
    VPSHAL_EDE_PEAKING_COEFF_1_2,
    VPSHAL_EDE_PEAKING_COEFF_1_5,
    VPSHAL_EDE_PEAKING_COEFF_1_8,
    VPSHAL_EDE_PEAKING_COEFF_2,
    VPSHAL_EDE_PEAKING_COEFF_2_4,
    VPSHAL_EDE_PEAKING_COEFF_3,
    VPSHAL_EDE_PEAKING_COEFF_3_4,
    VPSHAL_EDE_PEAKING_COEFF_4,
    VPSHAL_EDE_PEAKING_COEFF_4_8,
    VPSHAL_EDE_PEAKING_COEFF_6,
} VpsHal_EdePeakingCoeff;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Ede_Info
 *  \brief Structure containing Instance Information for each EDE module
 *
 */
typedef struct
{
    UInt32                  instId;
    /**< Indicates the path on which EDE is available */
    UInt32                  openCnt;
    /**< Keeps track of number of open for a Ede instance */
    CSL_Vps_edeRegs        *regOvly;
    /**< Pointer to the CSLR register overlay structure */
    UInt32                  configOvlySize;
    /**< Virtual register overlay size in terms of bytes. Used to allocate
         memory for virtual register configured through VPDMA */
    UInt32                 *regOffset[EDE_CONFIG_OVLY_NUM_IDX];
    /**< Array of physical address of the register configured through VPDMA
         register overlay.*/
    UInt32                  virtRegOffset[EDE_CONFIG_OVLY_NUM_IDX];
    /**< Array of indexes into the Overlay memory where virtual registers are
         to be configured */
} Ede_Info;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int edeSetExpertConfig(CSL_Vps_edeRegs *regOvly);
static Void edeSetPeakingConfig(VpsHal_Handle handle,
                                VpsHal_EdePeakingConfig *pCfg);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 * \brief EDE objects - Module variable to store information about each EDE
 * instance.
 */
static Ede_Info EdeObj[VPSHAL_EDE_MAX_INST] =
{
    {VPSHAL_EDE_INST_0, 0, NULL, 0, {NULL}, {0}}
};

VpsHal_EdePeakingConfig gVpsHalPeakingCfg[] = EDE_PEAKING_EXPERT_VAL;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_edeInit
 *  \brief EDE HAL init function.
 *
 *  Initializes EDE objects, gets the register overlay offsets for EDE
 *  registers.
 *  This function should be called before calling any of EDE HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of EDE HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *  \param numInstances NONE
 *  \param initParams   NONE
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_edeInit(UInt32 numInstances,
                   VpsHal_EdeInitParams *initParams,
                   Ptr arg)
{
    Int                  instCnt;
    CSL_Vps_edeRegs     *regOvly = NULL;
    Int                  ret = 0;
    Ede_Info            *instInfo = NULL;

    GT_assert( GT_DEFAULT_MASK, NULL != initParams);
    GT_assert( GT_DEFAULT_MASK, numInstances <= VPSHAL_EDE_MAX_INST);

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < VPSHAL_EDE_MAX_INST; instCnt ++)
    {
        GT_assert( GT_DEFAULT_MASK, initParams[instCnt].instId < VPSHAL_EDE_MAX_INST);

        /* Initialize Instance Specific Parameters */
        instInfo = &(EdeObj[instCnt]);

        instInfo->instId = initParams[instCnt].instId;
        instInfo->regOvly = (CSL_Vps_edeRegs *)initParams[instCnt].baseAddress;

        /* Calculate VPDMA register overlay size and register offset in
         * the VPDMA register overlay. Since register organization for all
         * EDE are same, only one array of register offset is sufficient. */
        /* Since the register offset are same for all EDE instances,
         * using one EDE to get physical offset. */
        regOvly = instInfo->regOvly;

        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE1_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE1);
        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE3_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE3);
        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE4_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE4);
        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE5_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE5);
        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE16_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE16);
        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE17_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE17);
        instInfo->regOffset[EDE_CONFIG_OVLY_CFG_EDE18_IDX] =
                                        (UInt32 *)&(regOvly->CFG_EDE18);

        instInfo->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                    instInfo->regOffset,
                                    EDE_CONFIG_OVLY_NUM_IDX,
                                    instInfo->virtRegOffset);
        edeSetExpertConfig(regOvly);
        edeSetPeakingConfig((VpsHal_Handle )instInfo,
                            &gVpsHalPeakingCfg[VPSHAL_EDE_PEAKING_COEFF_1]);
    }

    return (ret);
}



/**
 *  VpsHal_edeDeInit
 *  \brief EDE HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_edeDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_edeOpen
 *  \brief Returns the handle to the requested EDE instance.
 *
 *  This function should be called prior to calling any of the EDE HAL
 *  configuration APIs to get the instance handle.
 *
 *  VpsHal_edeInit should be called prior to this.
 *
 *  \param edeInst Requested EDE instance.
 *
 *  \return        Returns EDE instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_edeOpen(UInt32 edeInst)
{
    Int                     cnt;
    UInt32                  cookie;
    VpsHal_Handle           handle = NULL;
    Ede_Info               *instInfo = NULL;

    for (cnt = 0; cnt < VPSHAL_EDE_MAX_INST; cnt++)
    {
        instInfo = &(EdeObj[cnt]);
        /* Return the matching instance handle */
        if (edeInst == instInfo->instId)
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
 *  VpsHal_edeClose
 *  \brief Closes the EDE HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case in the
 *  future resource management is done by individual HAL - using counters.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_edeOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_edeClose(VpsHal_Handle handle)
{
    Int            ret = -1;
    UInt32         cookie;
    Ede_Info      *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));

    instInfo = (Ede_Info *) handle;

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
 *  VpsHal_edeGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the EDE registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register overlay
 *  used to program the EDE registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving valuable
 *  CPU in programming the VPS registers.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_edeOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
UInt32 VpsHal_edeGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32 configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((Ede_Info *) handle)->configOvlySize;

    return (configOvlySize);
}



/**
 *  VpsHal_edeCreateConfigOvly
 *  \brief Creates the EDE register configuration overlay using VPDMA helper
 *  function.
 *
 *  This function does not initialize the overlay with EDE configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  VpsHal_edeSetConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle         Valid handle returned by VpsHal_edeOpen function.
 *
 *  \param configOvlyPtr  Pointer to the overlay memory where the overlay is
 *                        formed. The memory for the overlay should be
 *                        allocated by client driver.
 *                        This parameter should be non-NULL.
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_edeCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int ret;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));
    GT_assert( GT_DEFAULT_MASK, (NULL != configOvlyPtr));

    /* Create the register overlay */
    ret = VpsHal_vpdmaCreateRegOverlay(
              ((Ede_Info *) handle)->regOffset,
              EDE_CONFIG_OVLY_NUM_IDX,
              configOvlyPtr);

    return (ret);
}



/**
 *  VpsHal_edeSetConfig
 *  \brief Sets the EDE configuration to either the
 *  actual EDE registers or to the configuration overlay memory.
 *
 *  This function configures the EDE registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual EDE MMR or
 *  to configuration overlay memory.
 *  This function configures all the sub-modules in EDE using other config
 *  function call.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  EDE registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle        Valid handle returned by VpsHal_edeOpen function.
 *
 *  \param config        Pointer to VpsHal_EdeConfig structure containing the
 *                       register configurations. This parameter should be
 *                       non-NULL.
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual EDE registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_edeSetConfig(VpsHal_Handle handle,
                        const VpsHal_EdeConfig *config,
                        Ptr configOvlyPtr)
{
    Int              ret = 0;
    CSL_Vps_edeRegs *regOvly = NULL;
    Ede_Info        *instInfo = NULL;
    UInt32           tempReg18 = 0u;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));
    GT_assert( GT_DEFAULT_MASK, (NULL != config));

    instInfo = (Ede_Info *) handle;
    regOvly = instInfo->regOvly;

    if (FALSE == config->bypass)
    {
        tempReg18 &= (~CSL_VPS_EDE_CFG_EDE18_CFG_BYP_MASK);

        regOvly->CFG_EDE1 &= (~CSL_VPS_EDE_CFG_EDE1_CFG_LTI_MODE_3_MASK);
        if (TRUE == config->ltiEnable)
        {
            regOvly->CFG_EDE1 |=
                (1u << CSL_VPS_EDE_CFG_EDE1_CFG_LTI_MODE_3_SHIFT);
        }

        regOvly->CFG_EDE1 &= (~CSL_VPS_EDE_CFG_EDE1_CFG_LTI_PEAKING_EN_B_MASK);
        if (TRUE == config->horzPeaking)
        {
            regOvly->CFG_EDE1 |=
                    (CSL_VPS_EDE_CFG_EDE1_CFG_LTI_PEAKING_EN_B_MASK);
        }

        regOvly->CFG_EDE3 &= (~CSL_VPS_EDE_CFG_EDE3_CFG_CTI_MODE_3_MASK);
        if (TRUE == config->ctiEnable)
        {
            regOvly->CFG_EDE3 |=
                (2u << CSL_VPS_EDE_CFG_EDE3_CFG_CTI_MODE_3_SHIFT);
        }

        regOvly->CFG_EDE4 &=
                    (~CSL_VPS_EDE_CFG_EDE4_CFG_CTI_EN_TRANS_ADJUST_B_MASK);
        if (TRUE == config->transAdjustEnable)
        {
            regOvly->CFG_EDE4 |=
                    (CSL_VPS_EDE_CFG_EDE4_CFG_CTI_EN_TRANS_ADJUST_B_MASK);
        }

        regOvly->CFG_EDE5 &=
                    (~(CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_Y_EN_B_MASK |
                       CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_C_EN_B_MASK |
                       CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_ALG_2_MASK |
                       CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_CLIP_SUPPRESSION_EN_B_MASK));
        if (TRUE == config->lumaPeaking)
        {
            regOvly->CFG_EDE5 |=
                    (CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_Y_EN_B_MASK);
        }
        if (TRUE == config->chromaPeaking)
        {
            regOvly->CFG_EDE5 |=
                    (CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_C_EN_B_MASK);
        }
        if (TRUE == config->peakingClipSuppressEnable)
        {
            regOvly->CFG_EDE5 |=
                (CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_CLIP_SUPPRESSION_EN_B_MASK);
        }
        if (VPSHAL_EDE_PEAK_VERTICAL == config->peakAlgo)
        {
            regOvly->CFG_EDE5 |=
                    (1u << CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_ALG_2_SHIFT);
        }
        regOvly->CFG_EDE16 = ((config->maxClipLuma <<
                        CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MAX_Y_10_SHIFT) &
                        CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MAX_Y_10_MASK) |
                        ((config->minClipLuma <<
                        CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MIN_Y_10_SHIFT) &
                        CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MIN_Y_10_MASK);
        regOvly->CFG_EDE17 = ((config->maxClipChroma <<
                        CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MAX_C_10_SHIFT) &
                        CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MAX_C_10_MASK) |
                        ((config->minClipChroma <<
                        CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MIN_C_10_SHIFT) &
                        CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MIN_C_10_MASK);
        tempReg18 |= (((config->vemoMode <<
                                CSL_VPS_EDE_CFG_EDE18_CFG_VEMO_SHIFT) &
                                CSL_VPS_EDE_CFG_EDE18_CFG_VEMO_MASK) |
                               ((config->width <<
                                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_H_12_SHIFT) &
                                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_H_12_MASK) |
                               ((config->height <<
                                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_V_12_SHIFT) &
                                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_V_12_MASK));
    }
    else
    {
        tempReg18 |= CSL_VPS_EDE_CFG_EDE18_CFG_BYP_MASK;
    }
    regOvly->CFG_EDE18 = tempReg18;

    return (ret);
}



/**
 *  VpsHal_edeGetConfig
 *  \brief Gets the EDE configuration from the actual EDE registers.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_edeOpen function.
 *
 *  \param config Pointer to VpsHal_EdeConfig structure to be filled with
 *                register configurations. This parameter should be non-NULL.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_edeGetConfig(VpsHal_Handle handle, VpsHal_EdeConfig *config)
{
    CSL_Vps_edeRegs    *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));
    GT_assert( GT_DEFAULT_MASK, (NULL != config));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = ((Ede_Info *) handle)->regOvly;

    config->ltiEnable = (regOvly->CFG_EDE1 &
                            CSL_VPS_EDE_CFG_EDE1_CFG_LTI_MODE_3_MASK) >>
                            CSL_VPS_EDE_CFG_EDE1_CFG_LTI_MODE_3_SHIFT;
    config->horzPeaking = (regOvly->CFG_EDE1 &
                            CSL_VPS_EDE_CFG_EDE1_CFG_LTI_PEAKING_EN_B_MASK) >>
                            CSL_VPS_EDE_CFG_EDE1_CFG_LTI_PEAKING_EN_B_SHIFT;
    config->ctiEnable = (regOvly->CFG_EDE3 &
                            CSL_VPS_EDE_CFG_EDE3_CFG_CTI_MODE_3_MASK) >>
                            CSL_VPS_EDE_CFG_EDE3_CFG_CTI_MODE_3_SHIFT;
    if (config->ctiEnable > 0u)
    {
        config->ctiEnable = TRUE;
    }
    config->transAdjustEnable = (regOvly->CFG_EDE4 &
                        CSL_VPS_EDE_CFG_EDE4_CFG_CTI_EN_TRANS_ADJUST_B_MASK) >>
                        CSL_VPS_EDE_CFG_EDE4_CFG_CTI_EN_TRANS_ADJUST_B_SHIFT;

    config->lumaPeaking = (regOvly->CFG_EDE5 &
                        CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_Y_EN_B_MASK) >>
                        CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_Y_EN_B_SHIFT;
    config->chromaPeaking = (regOvly->CFG_EDE5 &
                        CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_C_EN_B_MASK) >>
                        CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_C_EN_B_SHIFT;
    config->peakAlgo = (VpsHal_EdePeaking)((regOvly->CFG_EDE5 &
                        CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_ALG_2_MASK) >>
                        CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_ALG_2_SHIFT);
    config->peakingClipSuppressEnable = (regOvly->CFG_EDE5 &
                CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_CLIP_SUPPRESSION_EN_B_MASK) >>
                CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_CLIP_SUPPRESSION_EN_B_SHIFT;
    config->vemoMode = (VpsHal_EdeVemoMode)((regOvly->CFG_EDE18 &
                CSL_VPS_EDE_CFG_EDE18_CFG_VEMO_MASK) >>
                CSL_VPS_EDE_CFG_EDE18_CFG_VEMO_SHIFT);
    config->minClipLuma = (regOvly->CFG_EDE16 &
                CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MIN_Y_10_MASK) >>
                CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MIN_Y_10_SHIFT;
    config->maxClipLuma = (regOvly->CFG_EDE16 &
                CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MAX_Y_10_MASK) >>
                CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MAX_Y_10_SHIFT;
    config->minClipChroma = (regOvly->CFG_EDE17 &
                CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MIN_C_10_MASK) >>
                CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MIN_C_10_SHIFT;
    config->maxClipChroma = (regOvly->CFG_EDE17 &
                CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MAX_C_10_MASK) >>
                CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MAX_C_10_SHIFT;
    config->width = (regOvly->CFG_EDE18 &
                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_H_12_MASK) >>
                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_H_12_SHIFT;
    config->height = (regOvly->CFG_EDE18 &
                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_V_12_MASK) >>
                CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_V_12_SHIFT;
    config->bypass = (regOvly->CFG_EDE18 &
                CSL_VPS_EDE_CFG_EDE18_CFG_BYP_MASK) >>
                CSL_VPS_EDE_CFG_EDE18_CFG_BYP_SHIFT;

    return (0);
}



/**
 * edeSetExpertConfig
 * \brief Sets expert configuration.
 */
static Int edeSetExpertConfig(CSL_Vps_edeRegs *regOvly)
{
    Ede_ExpertConfig expertConfig = EDE_DEFAULT_EXPERT_VAL;
    regOvly->CFG_EDE0 = expertConfig.cfgEde0;
    regOvly->CFG_EDE1 = expertConfig.cfgEde1;
    regOvly->CFG_EDE2 = expertConfig.cfgEde2;
    regOvly->CFG_EDE3 = expertConfig.cfgEde3;
    regOvly->CFG_EDE4 = expertConfig.cfgEde4;
    regOvly->CFG_EDE5 = expertConfig.cfgEde5;
    regOvly->CFG_EDE6 = expertConfig.cfgEde6;
    regOvly->CFG_EDE7 = expertConfig.cfgEde7;
    regOvly->CFG_EDE8 = expertConfig.cfgEde8;
    regOvly->CFG_EDE9 = expertConfig.cfgEde9;
    regOvly->CFG_EDE10 = expertConfig.cfgEde10;
    regOvly->CFG_EDE11 = expertConfig.cfgEde11;
    regOvly->CFG_EDE12 = expertConfig.cfgEde12;
    regOvly->CFG_EDE13 = expertConfig.cfgEde13;
    regOvly->CFG_EDE14 = expertConfig.cfgEde14;
    regOvly->CFG_EDE15 = expertConfig.cfgEde15;
    regOvly->CFG_EDE16 = expertConfig.cfgEde16;
    regOvly->CFG_EDE17 = expertConfig.cfgEde17;
    regOvly->CFG_EDE18 = expertConfig.cfgEde18;
    regOvly->CFG_EDE19 = expertConfig.cfgEde19;
    regOvly->CFG_EDE20 = expertConfig.cfgEde20;
    regOvly->CFG_EDE21 = expertConfig.cfgEde21;
    return (0);
}



/**
 * edeSetPeakingConfig
 * \brief Sets Peaking Configuration.
 */
static Void edeSetPeakingConfig(VpsHal_Handle handle,
                                VpsHal_EdePeakingConfig *pCfg)
{
    CSL_Vps_edeRegs    *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));
    GT_assert( GT_DEFAULT_MASK, (NULL != pCfg));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = ((Ede_Info *) handle)->regOvly;

    regOvly->CFG_EDE6 = ((pCfg->hpfGain[0u] <<
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN0_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN0_S8_MASK) |
        ((pCfg->hpfGain[1u] <<
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN1_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN1_S8_MASK) |
        ((pCfg->hpfGain[2u] <<
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN2_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN2_S8_MASK) |
        ((pCfg->hpfGain[3u] <<
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN3_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN3_S8_MASK);

    regOvly->CFG_EDE7 = ((pCfg->hpfGain[4u] <<
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN4_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN4_S8_MASK) |
        ((pCfg->hpfGain[5u] <<
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN5_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN5_S8_MASK) |
        ((pCfg->hpfGain[6u] <<
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN6_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN6_S8_MASK) |
        ((pCfg->hpfGain[7u] <<
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN7_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN7_S8_MASK);

    regOvly->CFG_EDE8 = ((pCfg->yThrPLow <<
        CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_Y_TABLE_TH_P_LOW_S10_SHIFT) &
        CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_Y_TABLE_TH_P_LOW_S10_MASK) |
        ((pCfg->hpfGain8 <<
        CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_GAIN8_S8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_GAIN8_S8_MASK) |
        ((pCfg->hpfNormShift <<
        CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_NORMARIZE_SHIFT_3_SHIFT) &
        CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_NORMARIZE_SHIFT_3_MASK);

    regOvly->CFG_EDE9 = ((pCfg->yThrNLow <<
        CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_N_LOW_S10_SHIFT) &
        CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_N_LOW_S10_MASK) |
        ((pCfg->yThrPHigh <<
        CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_P_HIGH_S10_SHIFT) &
        CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_P_HIGH_S10_MASK);

    regOvly->CFG_EDE10 = ((pCfg->yThrNHigh <<
        CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_TH_N_HIGH_S10_SHIFT) &
        CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_TH_N_HIGH_S10_MASK) |
        ((pCfg->ySlopePLowx16 <<
        CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_P_LOW_X16_8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_P_LOW_X16_8_MASK) |
        ((pCfg->ySlopeNLowx16 <<
        CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_N_LOW_X16_8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_N_LOW_X16_8_MASK);

    regOvly->CFG_EDE11 = ((pCfg->VDiffYMax <<
        CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_V_DIFF_Y_MAX_10_SHIFT) &
        CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_V_DIFF_Y_MAX_10_MASK) |
        ((pCfg->ySlopePHighShift3 <<
        CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_P_HIGH_SHIFT_3_SHIFT) &
        CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_P_HIGH_SHIFT_3_MASK) |
        ((pCfg->ySlopeNHighShift3 <<
        CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_N_HIGH_SHIFT_3_SHIFT) &
        CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_N_HIGH_SHIFT_3_MASK);

    regOvly->CFG_EDE12 = ((pCfg->gainEdgeMaxYx16 <<
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_MAX_Y_X16_4_SHIFT) &
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_MAX_Y_X16_4_MASK) |
        ((pCfg->gainEdgeSlopeYx16 <<
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_SLOPE_Y_X16_4_SHIFT) &
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_SLOPE_Y_X16_4_MASK) |
        ((pCfg->gainPeakYx8 <<
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_Y_X8_6_SHIFT) &
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_Y_X8_6_MASK) |
        ((pCfg->gainPeakCx8 <<
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_C_X8_6_SHIFT) &
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_C_X8_6_MASK) |
        ((pCfg->gainHorizontalSlopex16 <<
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_HORIZONTAL_SLOPE_X16_5_SHIFT) &
        CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_HORIZONTAL_SLOPE_X16_5_MASK);

    regOvly->CFG_EDE13 = ((pCfg->yMax <<
        CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MAX_9_SHIFT) &
        CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MAX_9_MASK) |
        ((pCfg->yMin <<
        CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MIN_S10_SHIFT) &
        CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MIN_S10_MASK);

    regOvly->CFG_EDE14 = ((pCfg->cSlopePx512 <<
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_P_X512_8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_P_X512_8_MASK) |
        ((pCfg->cSlopeNx512 <<
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_N_X512_8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_N_X512_8_MASK) |
        ((pCfg->yNegCoringGain8 <<
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_GAIN_8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_GAIN_8_MASK) |
        ((pCfg->yNegCoringLimit8 <<
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_LIMIT_8_SHIFT) &
        CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_LIMIT_8_MASK);

    regOvly->CFG_EDE15 = ((pCfg->cGainMaxx512 <<
        CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MAX_X512_11_SHIFT) &
        CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MAX_X512_11_MASK) |
        ((pCfg->cGainMinx512 <<
        CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MIN_X512_11_SHIFT) &
        CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MIN_X512_11_MASK);
}
