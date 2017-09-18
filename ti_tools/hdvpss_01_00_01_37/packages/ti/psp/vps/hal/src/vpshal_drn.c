/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_drn.c
 *
 * \brief VPS De-Ringing HAL file.
 * This file implements the HAL APIs of the VPS De-Ringer.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/hal/vpshal_drn.h>
#include <ti/psp/cslr/cslr_vps_drn.h>
#include <ti/psp/vps/hal/src/vpshalDrnDefaults.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief enum Drn_ConfigOvlyReg
 * Enums for the configuration overlay register offsets - used as array indices
 * to register offset array in Drn_Obj object. These enums defines the
 * registers that will be used to form the configuration register overlay for
 * DRN module. This in turn is used by client drivers to configure registers
 * using VPDMA during context-switching (software channels with different
 * configurations).
 *
 * Note: If in future it is decided to remove or add some register, this enum
 * should be modified accordingly. And make sure that the values of these
 * enums are in ascending order and it starts from 0.
 */
typedef enum
{
    DRN_CONFIG_OVLY_DRN_CTRL_IDX = 0,
    DRN_CONFIG_OVLY_DRN_SIZE_IDX,
    DRN_CONFIG_OVLY_NUM_IDX        /* This should be the last enum */
} Drn_ConfigOvlyReg;

/*
 * Macros for maximum value possible for some of the bit fields used.
 */
#define DRN_MAX_DRN_WIDTH                                                     \
                    (CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_MASK >>                   \
                     CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_SHIFT)
#define DRN_MAX_DRN_HEIGHT                                                    \
                    (CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_MASK >>                  \
                     CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_SHIFT)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief struct Drn_Info
 * Structure containing Instance Information for each DRN module
 *
 * @instId = Indicates the path on which DRN is available
 * @regOvly - Pointer to the CSLR register overlay structure
 * @openCnt - Keeps track of number of open for a Drn instance
 * @configOvlySize - Virtual register overlay size in terms of bytes. Used
 *                   to allocate memory for virtual register configured
 *                   through VPDMA
 * @regOffset - Array of physical address of the register configured through
 *              VPDMA register overlay.
 * @virtRegOffset - Array of indexes into the Overlay memory where virtual
 *                  registers are to be configured
 */
typedef struct
{
    UInt32              instId;
    UInt32              openCnt;
    CSL_VpsDrnRegsOvly  regOvly;
    UInt32              configOvlySize;
    UInt32             *regOffset[DRN_CONFIG_OVLY_NUM_IDX];
    UInt32              virtRegOffset[DRN_CONFIG_OVLY_NUM_IDX];
} Drn_Info;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int drnSetExpertConfig(Drn_Info *drnInstInfo,
                              Drn_DefaultConfig *defConfig);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*
 * DRN objects - Module variable to store information about each DRN instance.
 * Note: If the number of DRN instance increase, then this variable should be
 * initialized with the added DRN instance information.
 */
static Drn_Info DrnObj[VPSHAL_DRN_MAX_INST];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* DRN HAL init function */
Int VpsHal_drnInit(UInt32 numInstances,
                   const VpsHal_DrnInstParams *initParams,
                   Ptr arg)
{
    Int                 instCnt;
    CSL_VpsDrnRegsOvly  regOvly = NULL;
    Int                 ret = 0;
    Drn_Info           *instInfo = NULL;
    Drn_DefaultConfig   defConfig = DRN_EXPERT_VAL_DEFAULT_CONFIG;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_DRN_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < numInstances; instCnt ++)
    {
        GT_assert(VpsHalTrace,
            (initParams[instCnt].instId < VPSHAL_DRN_MAX_INST));

        /* Initialize Instance Specific Parameters */
        instInfo = &(DrnObj[instCnt]);
        instInfo->openCnt = 0u;
        instInfo->regOvly =
                (CSL_VpsDrnRegsOvly ) initParams[instCnt].baseAddress;
        instInfo->instId = initParams[instCnt].instId;
        drnSetExpertConfig(instInfo, &defConfig);

        /* Calculate VPDMA register overlay size and register offset in
         * the VPDMA register overlay. Since register organization for all
         * DRN are same, only one array of register offset is sufficient. */
        /* Since the register offset are same for all DRN instances, using HQ
         * DRN to get physical offset. */
        regOvly = instInfo->regOvly;
        instInfo->regOffset[DRN_CONFIG_OVLY_DRN_CTRL_IDX] =
                                                (UInt32 *)&(regOvly->DRN_CTRL);
        instInfo->regOffset[DRN_CONFIG_OVLY_DRN_SIZE_IDX] =
                                                (UInt32 *)&(regOvly->DRN_SIZE);

        instInfo->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                    instInfo->regOffset,
                                    DRN_CONFIG_OVLY_NUM_IDX,
                                    instInfo->virtRegOffset);
    }
    return (ret);
}



/* DRN HAL exit function */
Int VpsHal_drnDeInit(Ptr arg)
{
    return (0);
}



/* Returns the handle to the requested DRN instance */
VpsHal_Handle VpsHal_drnOpen(UInt32 drnInst)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;
    Drn_Info           *instInfo = NULL;

    for (cnt = 0; cnt < VPSHAL_DRN_MAX_INST; cnt++)
    {
        instInfo = &(DrnObj[cnt]);
        /* Return the matching instance handle */
        if (drnInst == instInfo->instId)
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



/* Closes the DRN HAL instance */
Int VpsHal_drnClose(VpsHal_Handle handle)
{
    Int            ret = -1;
    UInt32         cookie;
    Drn_Info       *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    instInfo = (Drn_Info *) handle;

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



/* Returns the size (in bytes) of the VPDMA register configuration overlay
 * required to program the DRN registers using VPDMA config descriptors */
UInt32 VpsHal_drnGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32 configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((Drn_Info *) handle)->configOvlySize;

    return (configOvlySize);
}



/* Creates the DRN register configuration overlay */
Int VpsHal_drnCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int ret;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    ret = VpsHal_vpdmaCreateRegOverlay(
              ((Drn_Info *) handle)->regOffset,
              DRN_CONFIG_OVLY_NUM_IDX,
              configOvlyPtr);

    return (ret);
}



/* Sets the entire DRN configuration to either the actual DRN registers or to
 * the configuration overlay memory */
Int VpsHal_drnSetConfig(VpsHal_Handle handle,
                        const VpsHal_DrnConfig *config,
                        Ptr configOvlyPtr)
{
    Int                 ret = 0;
    UInt32              ctrlReg = 0u, sizeReg = 0u;
    CSL_VpsDrnRegsOvly  regOvly = NULL;
    Drn_Info           *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->width <= DRN_MAX_DRN_WIDTH));
    GT_assert(VpsHalTrace, (config->height <= DRN_MAX_DRN_HEIGHT));

    instInfo = (Drn_Info *) handle;
    regOvly = instInfo->regOvly;

    /* Set the bypass mode in the ctrl register */
    ctrlReg = 0u;
    if (FALSE == config->bypass)
    {
        ctrlReg |= CSL_VPS_DRN_DRN_CTRL_CFG_EN_DRN_MASK;
    }
    /* Set the frame format in the ctrl registe r*/
    if (VPS_SF_PROGRESSIVE == config->scanFormat)
    {
        ctrlReg |= CSL_VPS_DRN_DRN_CTRL_CFG_PROGRESSIVE_MASK;
    }
    /* Set the edge block Horizontal Dilation mode in ctrl register */
    if (VPSHAL_DRN_TILE_SIZE_16x8 == config->dilateTileSize)
    {
        ctrlReg |= CSL_VPS_DRN_DRN_CTRL_CFG_EN_EDGE_BLCK_HOR_DILATION_MASK;
    }

    sizeReg = (config->width << CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_SHIFT) &
                 CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_MASK;
    sizeReg |= (config->height << CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_SHIFT) &
                 CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_MASK;

    if (NULL == configOvlyPtr)
    {
        /* Set the Configuration in Hardware Register */
        regOvly->DRN_CTRL = ctrlReg;
        regOvly->DRN_SIZE = sizeReg;
    }
    else /* Non Null Overlay Pointer */
    {
        /* Set the Configuration in the Overlay Virtual Register */
        *((UInt32 *)configOvlyPtr +
            instInfo->virtRegOffset[DRN_CONFIG_OVLY_DRN_CTRL_IDX]) = ctrlReg;
        *((UInt32 *)configOvlyPtr +
            instInfo->virtRegOffset[DRN_CONFIG_OVLY_DRN_SIZE_IDX]) = sizeReg;
    }

    return (ret);
}



/* Gets the entire DRN configuration from the actual DRN registers */
Int VpsHal_drnGetConfig(VpsHal_Handle handle, VpsHal_DrnConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDrnRegsOvly  regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = ((Drn_Info *) handle)->regOvly;

    /* Get the control Register */
    tempReg = regOvly->DRN_CTRL;

    if (0 != (tempReg & CSL_VPS_DRN_DRN_CTRL_CFG_EN_DRN_MASK))
    {
        config->bypass = FALSE;
    }
    else /* Bypass Bit is not set */
    {
        config->bypass = TRUE;
    }

    if (0 != (tempReg & CSL_VPS_DRN_DRN_CTRL_CFG_PROGRESSIVE_MASK))
    {
        config->scanFormat = VPS_SF_PROGRESSIVE;
    }
    else /* Interlaced Frame */
    {
        config->scanFormat = VPS_SF_INTERLACED;
    }

    config->dilateTileSize = (VpsHal_DrnHorzDilateTileSize) ((tempReg &
        CSL_VPS_DRN_DRN_CTRL_CFG_EN_EDGE_BLCK_HOR_DILATION_MASK) >>
        CSL_VPS_DRN_DRN_CTRL_CFG_EN_EDGE_BLCK_HOR_DILATION_SHIFT);

    tempReg = regOvly->DRN_SIZE;
    config->width = (tempReg & CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_MASK) >>
        CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_SHIFT;
    config->height = (tempReg & CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_MASK) >>
        CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_SHIFT;

    return (0);
}



/**
 * Sets the expert configurations in the DRN registers.
 */
static Int drnSetExpertConfig(Drn_Info *instInfo,
                              Drn_DefaultConfig *defConfig)
{
    UInt32              tempReg;
    CSL_VpsDrnRegsOvly  regOvly = NULL;

    GT_assert(VpsHalTrace, (NULL != instInfo));
    GT_assert(VpsHalTrace, (NULL != defConfig));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = instInfo->regOvly;

    tempReg = (defConfig->thrNumEdge <<
                CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_MASK;
    tempReg |= (defConfig->thrNumEdgeStrong <<
                CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_STRONG_SHIFT) &
                CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_STRONG_MASK;
    tempReg |= (defConfig->thrNumFlatpixBlck <<
                CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_FLATPIX_BLCK_SHIFT) &
                CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_FLATPIX_BLCK_MASK;
    regOvly->THR_CFG0 = tempReg;

    tempReg = (defConfig->thrStrongEdge <<
                CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_STRONGEDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_STRONGEDGE_MASK;
    tempReg |= (defConfig->thrWeakEdge <<
                CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_WEAKEDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_WEAKEDGE_MASK;
    regOvly->THR_CFG1 = tempReg;

    tempReg = (defConfig->thrVeryFlatEdge <<
                CSL_VPS_DRN_THR_CFG2_CFG_THR_SAD_VERYFLAT_SHIFT) &
                CSL_VPS_DRN_THR_CFG2_CFG_THR_SAD_VERYFLAT_MASK;
    tempReg |= (defConfig->thrFlatEdge <<
                CSL_VPS_DRN_THR_CFG2_CFG_THR_GSUM_FLAT_SHIFT) &
                CSL_VPS_DRN_THR_CFG2_CFG_THR_GSUM_FLAT_MASK;
    regOvly->THR_CFG2 = tempReg;

    tempReg = (defConfig->maxBldFactor <<
                CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_BLD_FACTOR_SHIFT) &
                CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_BLD_FACTOR_MASK;
    tempReg |= (defConfig->maxSigmaThr1 <<
                CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_SIGMA_THR1_SHIFT) &
                CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_SIGMA_THR1_MASK;
    tempReg |= (defConfig->bldCoeffQp <<
                CSL_VPS_DRN_THR_CFG3_CFG_DRN_BLD_COEFF_QP_SHIFT) &
                CSL_VPS_DRN_THR_CFG3_CFG_DRN_BLD_COEFF_QP_MASK;
    tempReg |= (defConfig->mpegQ <<
                CSL_VPS_DRN_THR_CFG3_CFG_MPEG_Q_SHIFT) &
                CSL_VPS_DRN_THR_CFG3_CFG_MPEG_Q_MASK;
    regOvly->THR_CFG3 = tempReg;

    tempReg = (defConfig->bldCoeffBlockEdge <<
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_BLOCK_EDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_BLOCK_EDGE_MASK;
    tempReg |= (defConfig->bldCoeffLocalEdge <<
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_LOCAL_EDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_LOCAL_EDGE_MASK;
    tempReg |= (defConfig->bldScaleWeakEdge <<
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_WEAKEDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_WEAKEDGE_MASK;
    tempReg |= (defConfig->bldScaleStrongEdge <<
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_STRGEDGE_SHIFT) &
                CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_STRGEDGE_MASK;
    regOvly->THR_CFG4 = tempReg;

    return (0);
}
