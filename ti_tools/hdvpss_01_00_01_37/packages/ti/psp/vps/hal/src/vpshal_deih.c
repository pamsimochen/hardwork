/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_deih.c
 *
 *  \brief VPS Deinterlacer High Quality HAL file.
 *  This file implements the HAL APIs of the VPS High Quality Deinterlacer.
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
#include <ti/psp/cslr/cslr_vps_dei_h.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_deih.h>
#include <ti/psp/vps/hal/src/vpshalDeihDefaults.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum VpsHal_DeihConfigOvlyIdx
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
    VPSHAL_DEIH_CONFIGOVLYIDX_DEI_CFG0 = 0,
    VPSHAL_DEIH_CONFIGOVLYIDX_DEI_CFG1,
    VPSHAL_DEIH_CONFIGOVLYIDX_MDT_CFG0,
    VPSHAL_DEIH_CONFIGOVLYIDX_EDI_CFG0,
    VPSHAL_DEIH_CONFIGOVLYIDX_TNR_CFG0,
    VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG0,
    VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG2,
    VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG3,
    VPSHAL_DEIH_CONFIGOVLYIDX_SNR_CFG0,
    VPSHAL_DEIH_NUM_REG_IDX             /* This should be the last enum */
} VpsHal_DeihConfigOvlyIdx;

/*
 * Masks for individual bit field for TNR Mode.
 */
#define VPSHAL_DEIH_TNR_CFG0_TNR_MODE_CHROMA_MASK   (0x00000001u)
#define VPSHAL_DEIH_TNR_CFG0_TNR_MODE_LUMA_MASK     (0x00000002u)
#define VPSHAL_DEIH_TNR_CFG0_TNR_MODE_ADAPTIVE_MASK (0x00000004u)

/*
 * Masks for individual bit field for GNR Mode.
 */
#define VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_CHROMA_MASK       (0x00000001u)
#define VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_LUMA_MASK         (0x00000002u)
#define VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_ADAPTIVE_MASK     (0x00000004u)

/*
 * Macros for maximum value possible for some of the bit fields used.
 */
#define VPSHAL_DEIH_MAX_DEI_WIDTH                                              \
                    (CSL_VPS_DEI_H_DEI_CFG0_WIDTH_MASK >>                      \
                     CSL_VPS_DEI_H_DEI_CFG0_WIDTH_SHIFT)
#define VPSHAL_DEIH_MAX_DEI_HEIGHT                                             \
                    (CSL_VPS_DEI_H_DEI_CFG0_HEIGHT_MASK >>                     \
                     CSL_VPS_DEI_H_DEI_CFG0_HEIGHT_SHIFT)
#define VPSHAL_DEIH_MAX_TNR_ADV_SCALE_FACT                                     \
                    (CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_SCALE_FACTOR_MASK >>  \
                     CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_SCALE_FACTOR_SHIFT)
#define VPSHAL_DEIH_MAX_TNR_MAX_GAIN_SKINTONE                                  \
                    (CSL_VPS_DEI_H_TNR_CFG0_TNR_MAX_GAIN_SKINTONE_MASK >>      \
                     CSL_VPS_DEI_H_TNR_CFG0_TNR_MAX_GAIN_SKINTONE_SHIFT)
#define VPSHAL_DEIH_MAX_TNR_SKT_MINC_THR                                       \
                    (CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_MINC_THR_MASK >>      \
                     CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_MINC_THR_SHIFT)
#define VPSHAL_DEIH_MAX_FMD_WINDOW_MINX                                        \
                    (CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINX_MASK >>            \
                     CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINX_SHIFT)
#define VPSHAL_DEIH_MAX_FMD_WINDOW_MINY                                        \
                    (CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINY_MASK >>            \
                     CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINY_SHIFT)
#define VPSHAL_DEIH_MAX_FMD_WINDOW_MAXX                                        \
                    (CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXX_MASK >>            \
                     CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXX_SHIFT)
#define VPSHAL_DEIH_MAX_FMD_WINDOW_MAXY                                        \
                    (CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXY_MASK >>            \
                     CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXY_SHIFT)
#define VPSHAL_DEIH_MAX_GNM_DEV_STBL_THR                                       \
                    (CSL_VPS_DEI_H_SNR_CFG0_GNM_DEV_STABLE_THR_MASK >>         \
                     CSL_VPS_DEI_H_SNR_CFG0_GNM_DEV_STABLE_THR_SHIFT)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_DeihObj
 *  \brief Per instance information about each DEIH module.
 */
typedef struct
{
    UInt32              instId;
    /**< Instance number/ID */
    UInt32              openCnt;
    /**< Counter to keep track of number of open calls for an instance */
    CSL_VpsDeihRegsOvly baseAddress;
    /**< Register overlay pointer */
    UInt32              configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay memory
         required */
    UInt32             *regOffset[VPSHAL_DEIH_NUM_REG_IDX];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA */
    UInt32              virRegOffset[VPSHAL_DEIH_NUM_REG_IDX];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to regOffset member */
} VpsHal_DeihObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 halDeihInitInstance(VpsHal_DeihObj *deihObj);

static Int32 halDeihSetExpertConfig(VpsHal_DeihObj *deihObj,
                                    const VpsHal_DeihExpertConfig *config);

static inline UInt32 halDeihMakeDeiCfg0Reg(UInt32 initRegVal,
                                           UInt16 width,
                                           UInt16 height);
static inline UInt32 halDeihMakeDeiCfg1Reg(UInt32 initRegVal,
                                           UInt32 scanFormat,
                                           UInt32 tempInpEnable,
                                           UInt32 tempInpChromaEnable,
                                           UInt32 fidPol,
                                           UInt32 bypass);
static inline UInt32 halDeihMakeMdtCfg0Reg(UInt32 initRegVal,
                                           UInt32 fldMode,
                                           UInt32 spatMaxBypass,
                                           UInt32 tempMaxBypass,
                                           UInt32 mvIIR,
                                           UInt32 lcModeEnable);
static inline UInt32 halDeihMakeEdiCfg0Reg(UInt32 initRegVal,
                                           UInt32 inpMode,
                                           UInt32 chromaEdiEnable);
static inline UInt32 halDeihMakeTnrCfg0Reg(UInt32 initRegVal,
                                           UInt32 chroma,
                                           UInt32 luma,
                                           UInt32 adaptive,
                                           UInt32 advMode,
                                           UInt8 advScaleFactor,
                                           UInt32 skt,
                                           UInt8 maxSktGain,
                                           UInt8 minChromaSktThrld);
static inline UInt32 halDeihMakeFmdCfg0Reg(UInt32 initRegVal,
                                           UInt32 filmMode,
                                           UInt32 bed,
                                           UInt32 window,
                                           UInt32 lock,
                                           UInt32 jamDir);
static inline UInt32 halDeihMakeFmdCfg2Reg(UInt32 initRegVal,
                                           UInt16 windowMinx,
                                           UInt16 windowMiny);
static inline UInt32 halDeihMakeFmdCfg3Reg(UInt32 initRegVal,
                                           UInt16 windowMaxx,
                                           UInt16 windowMaxy);
static inline UInt32 halDeihMakeSnrCfg0Reg(UInt32 initRegVal,
                                           UInt32 gnrChroma,
                                           UInt32 gnrLuma,
                                           UInt32 gnrAdaptive,
                                           UInt32 inrMode,
                                           UInt8 gnmDevStableThrld);
static inline UInt32 halDeihMakeMdtMiscCfg0Reg(UInt32 initRegVal,
                                               UInt32 adaptiveCoring,
                                               UInt32 fldComp,
                                               UInt32 edgeKd,
                                               UInt32 edgeVarThrld,
                                               UInt32 forceSlomoDiff,
                                               UInt32 forceF02Diff,
                                               UInt32 forceMv3D,
                                               UInt32 forceMv2D,
                                               UInt32 useDynGain,
                                               UInt32 enable3PixelFilt);

static Int32 vpsDeihSetAdvMdtCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqMdtConfig *mdtCfg);
static Int32 vpsDeihGetAdvMdtCfg(VpsHal_Handle handle,
                                 Vps_DeiHqMdtConfig *mdtCfg);
static Int32 vpsDeihSetAdvEdiCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqEdiConfig *ediCfg);
static Int32 vpsDeihGetAdvEdiCfg(VpsHal_Handle handle,
                                 Vps_DeiHqEdiConfig *ediCfg);
static Int32 vpsDeihSetAdvTnrCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqTnrConfig *tnrCfg);
static Int32 vpsDeihGetAdvTnrCfg(VpsHal_Handle handle,
                                 Vps_DeiHqTnrConfig *tnrCfg);
static Int32 vpsDeihSetAdvFmdCfg(VpsHal_Handle handle,
                                 const Vps_DeiFmdConfig *fmdCfg);
static Int32 vpsDeihGetAdvFmdCfg(VpsHal_Handle handle,
                                 Vps_DeiFmdConfig *fmdCfg);
static Int32 vpsDeihSetAdvSnrCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqSnrConfig *snrCfg);
static Int32 vpsDeihGetAdvSnrCfg(VpsHal_Handle handle,
                                 Vps_DeiHqSnrConfig *snrCfg);
static Int32 vpsDeihSetAdvGnmCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqGnmConfig *gnmCfg);
static Int32 vpsDeihGetAdvGnmCfg(VpsHal_Handle handle,
                                 Vps_DeiHqGnmConfig *gnmCfg);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  DEIH objects - Module variable to store information about each DEIH
 *  instance.
 *  Note: If the number of DEIH instance increase, then VPSHAL_DEIH_MAX_INST
 *  macro should be changed accordingly.
 */
static VpsHal_DeihObj DeihObjects[VPSHAL_DEIH_MAX_INST];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_deihInit
 *  \brief DEIH HAL init function.
 *
 *  Initializes DEIH objects, gets the register overlay offsets for DEIH
 *  registers.
 *  This function should be called before calling any of DEIH HAL API's.
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
Int VpsHal_deihInit(UInt32 numInstances,
                    const VpsHal_DeihInitParams *initParams,
                    Ptr arg)
{
    Int             instCnt;
    VpsHal_DeihObj *deihObj;
    Int             retVal = VPS_SOK;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_DEIH_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    /* Initialize DEI Objects to zero */
    VpsUtils_memset(DeihObjects, 0, sizeof(DeihObjects));

    for (instCnt = 0; instCnt < numInstances; instCnt++)
    {
        GT_assert(VpsHalTrace,
            (initParams[instCnt].instId < VPSHAL_DEIH_MAX_INST));

        deihObj = &DeihObjects[instCnt];
        deihObj->instId = initParams[instCnt].instId;
        deihObj->baseAddress =
            (CSL_VpsDeihRegsOvly) initParams[instCnt].baseAddress;

        /* Initialize the instance */
        retVal += halDeihInitInstance(deihObj);
    }

    return (retVal);
}



/**
 *  VpsHal_deihDeInit
 *  \brief DEIH HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg              Not used currently. Meant for future purpose
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int VpsHal_deihDeInit(Ptr arg)
{
    return (VPS_SOK);
}



/**
 *  VpsHal_deihOpen
 *  \brief Returns the handle to the requested DEIH instance.
 *
 *  This function should be called prior to calling any of the DEIH HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId           Requested DEIH instance.
 *
 *  \return                 Returns DEIH instance handle on success else
 *                          returns NULL.
 */
VpsHal_Handle VpsHal_deihOpen(UInt32 instId)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;

    /* Check if instance number is valid */
    GT_assert(VpsHalTrace, (instId < VPSHAL_DEIH_MAX_INST));

    for (cnt = 0; cnt < VPSHAL_DEIH_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (instId == DeihObjects[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0u == DeihObjects[cnt].openCnt)
            {
                handle = (VpsHal_Handle) &DeihObjects[cnt];
                DeihObjects[cnt].openCnt++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }

    return (handle);
}



/**
 *  VpsHal_deihClose
 *  \brief Closes the DEIH HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle           Valid handle returned by VpsHal_deihOpen function
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int VpsHal_deihClose(VpsHal_Handle handle)
{
    Int             retVal = VPS_EFAIL;
    UInt32          cookie;
    VpsHal_DeihObj *deihObj;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    deihObj = (VpsHal_DeihObj *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (deihObj->openCnt > 0u)
    {
        deihObj->openCnt--;
        retVal = VPS_SOK;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  VpsHal_deihGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the DEIH registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the DEIH registers using VPDMA.
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
UInt32 VpsHal_deihGetConfigOvlySize(VpsHal_Handle handle)
{
    UInt32          configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((VpsHal_DeihObj *) handle)->configOvlySize;

    return (configOvlySize);
}



/**
 *  VpsHal_deihCreateConfigOvly
 *  \brief Creates the DEIH register configuration overlay using VPDMA
 *  helper function.
 *
 *  This function does not initialize the overlay with DEIH configuration.
 *  It is the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_deihSetConfig function before submitting the same
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
Int32 VpsHal_deihCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr)
{
    Int32           retVal;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    retVal = VpsHal_vpdmaCreateRegOverlay(
                 ((VpsHal_DeihObj *) handle)->regOffset,
                 VPSHAL_DEIH_NUM_REG_IDX,
                 configOvlyPtr);

    return (retVal);
}



/**
 *  VpsHal_deihSetConfig
 *  \brief Sets the entire DEIH configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetConfig(VpsHal_Handle handle,
                           const VpsHal_DeihConfig *config,
                           Ptr configOvlyPtr)
{
    Int32               retVal;
    UInt32              height;
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->width <= VPSHAL_DEIH_MAX_DEI_WIDTH));
    GT_assert(VpsHalTrace, (config->height <= VPSHAL_DEIH_MAX_DEI_HEIGHT));
    /* If temporal Interpolation for luma is disabled, then temporal
     * interpolation for chroma should also be disabled */
    GT_assert(VpsHalTrace, (!((FALSE == config->tempInpEnable) &&
                                (TRUE == config->tempInpChromaEnable))));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Depending on the scanFormat, configure the height */
    if (VPS_SF_INTERLACED == config->scanFormat)
    {
        /* For interlaced scanformat, config parameters has field height. For
           Deinterlacing mode and interlaced bypass mode, DEI should
           be configured with the field size */
        height = config->height;
    }
    else
    {
        /* For progressive scanformat, config parameters has frame height. For
           progressive bypass mode, DEI should
           be configured with the half of the frame size */
        height = config->height / 2u;
    }
    /* Make register */
    tempReg = halDeihMakeDeiCfg0Reg(
                  regOvly->DEI_CFG0,
                  config->width,
                  height);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_DEI_CFG0];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halDeihMakeDeiCfg1Reg(
                  regOvly->DEI_CFG1,
                  config->scanFormat,
                  config->tempInpEnable,
                  config->tempInpChromaEnable,
                  config->fidPol,
                  config->bypass);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_CFG1 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_DEI_CFG1];
        *virRegOffset = tempReg;
    }

    /* Call individual functions to set the configuration */
    retVal = VpsHal_deihSetMdtConfig(handle, &config->mdt, configOvlyPtr);
    retVal += VpsHal_deihSetEdiConfig(handle, &config->edi, configOvlyPtr);
    retVal += VpsHal_deihSetTnrConfig(handle, &config->tnr, configOvlyPtr);
    retVal += VpsHal_deihSetFmdConfig(
                  handle,
                  &config->fmd,
                  configOvlyPtr,
                  config->width,
                  config->height);
    retVal += VpsHal_deihSetSnrConfig(handle, &config->snr, configOvlyPtr);

    return (retVal);
}



/**
 *  VpsHal_deihSetConfig
 *  \brief Sets Frame Size in DEIH register.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetFrameSize(VpsHal_Handle handle,
                              const VpsHal_DeihConfig *config,
                              Ptr configOvlyPtr)
{
    Int32               retVal = VPS_SOK;
    UInt32              height;
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->width <= VPSHAL_DEIH_MAX_DEI_WIDTH));
    GT_assert(VpsHalTrace, (config->height <= VPSHAL_DEIH_MAX_DEI_HEIGHT));

    /* If temporal Interpolation for luma is disabled, then temporal
     * interpolation for chroma should also be disabled */
    GT_assert(VpsHalTrace, (!((FALSE == config->tempInpEnable) &&
                                (TRUE == config->tempInpChromaEnable))));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Depending on the scanFormat, configure the height */
    if (VPS_SF_INTERLACED == config->scanFormat)
    {
        /* For interlaced scanformat, config parameters has field height. For
           Deinterlacing mode and interlaced bypass mode, DEI should
           be configured with the field size */
        height = config->height;
    }
    else
    {
        /* For progressive scanformat, config parameters has frame height. For
           progressive bypass mode, DEI should
           be configured with the half of the frame size */
        height = config->height / 2u;
    }
    /* Make register */
    tempReg = halDeihMakeDeiCfg0Reg(
                  regOvly->DEI_CFG0,
                  config->width,
                  height);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_CFG0 = tempReg;
    }
    else    /* Update in config overlay memory */
    {
        /* No Support for Writing Size through Configuration overlay */
    }

    /* Make register */
    tempReg = halDeihMakeDeiCfg1Reg(
                  regOvly->DEI_CFG1,
                  config->scanFormat,
                  config->tempInpEnable,
                  config->tempInpChromaEnable,
                  config->fidPol,
                  config->bypass);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->DEI_CFG1 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* No Support for Writing Size through Configuration overlay */
    }

    return (retVal);
}


/**
 *  VpsHal_deihSetMdtConfig
 *  \brief Sets the entire DEIH MDT configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH MDT registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetMdtConfig(VpsHal_Handle handle,
                              const VpsHal_DeihMdtConfig *config,
                              Ptr configOvlyPtr)
{
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Make register */
    tempReg = halDeihMakeMdtCfg0Reg(
                  regOvly->MDT_CFG0,
                  config->fldMode,
                  config->spatMaxBypass,
                  config->tempMaxBypass,
                  config->mvIIR,
                  config->lcModeEnable);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->MDT_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_MDT_CFG0];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihSetEdiConfig
 *  \brief Sets the entire DEIH EDI configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH EDI registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihEdiConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetEdiConfig(VpsHal_Handle handle,
                              const VpsHal_DeihEdiConfig *config,
                              Ptr configOvlyPtr)
{
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Make register */
    tempReg = halDeihMakeEdiCfg0Reg(
                  regOvly->EDI_CFG0,
                  config->inpMode,
                  config->chromaEdiEnable);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->EDI_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_EDI_CFG0];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihSetTnrConfig
 *  \brief Sets the entire DEIH TNR configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH TNR registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihTnrConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetTnrConfig(VpsHal_Handle handle,
                              const VpsHal_DeihTnrConfig *config,
                              Ptr configOvlyPtr)
{
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace, (config->advScaleFactor <= VPSHAL_DEIH_MAX_TNR_ADV_SCALE_FACT));
    GT_assert(VpsHalTrace, (config->maxSktGain <= VPSHAL_DEIH_MAX_TNR_MAX_GAIN_SKINTONE));
    GT_assert(VpsHalTrace, (config->minChromaSktThrld <= VPSHAL_DEIH_MAX_TNR_SKT_MINC_THR));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Make register */
    tempReg = halDeihMakeTnrCfg0Reg(
                  regOvly->TNR_CFG0,
                  config->chroma,
                  config->luma,
                  config->adaptive,
                  config->advMode,
                  config->advScaleFactor,
                  config->skt,
                  config->maxSktGain,
                  config->minChromaSktThrld);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->TNR_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_TNR_CFG0];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihSetFmdConfig
 *  \brief Sets the entire DEIH FMD configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH FMD registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihFmdConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *  \param width            Picture width at output.
 *  \param height           Picture height at output.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetFmdConfig(VpsHal_Handle handle,
                              const VpsHal_DeihFmdConfig *config,
                              Ptr configOvlyPtr,
                              UInt32 width,
                              UInt32 height)
{
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;
    UInt32              windowWidth;
    UInt32              windowHeight;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace,
        (config->windowMinx <= VPSHAL_DEIH_MAX_FMD_WINDOW_MINX));
    GT_assert(VpsHalTrace,
        (config->windowMiny <= VPSHAL_DEIH_MAX_FMD_WINDOW_MINY));
    GT_assert(VpsHalTrace,
        (config->windowMaxx <= VPSHAL_DEIH_MAX_FMD_WINDOW_MAXX));
    GT_assert(VpsHalTrace,
        (config->windowMaxy <= VPSHAL_DEIH_MAX_FMD_WINDOW_MAXY));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /*
     * Check whether the window size is less than the already set values
     */
    windowWidth = (config->windowMaxx - config->windowMinx) + 1u;
    windowHeight = (config->windowMaxy - config->windowMiny) + 1u;

    GT_assert(VpsHalTrace, (windowWidth <= width));
    GT_assert(VpsHalTrace, (windowHeight <= height));

    /* Make register */
    tempReg = halDeihMakeFmdCfg0Reg(
                  regOvly->FMD_CFG0,
                  config->filmMode,
                  config->bed,
                  config->window,
                  config->lock,
                  config->jamDir);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->FMD_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG0];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halDeihMakeFmdCfg2Reg(
                  regOvly->FMD_CFG2,
                  config->windowMinx,
                  config->windowMiny);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->FMD_CFG2 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG2];
        *virRegOffset = tempReg;
    }

    /* Make register */
    tempReg = halDeihMakeFmdCfg3Reg(
                  regOvly->FMD_CFG3,
                  config->windowMaxx,
                  config->windowMaxy);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->FMD_CFG3 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG3];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihSetSnrConfig
 *  \brief Sets the entire DEIH SNR configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH SNR registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihSnrConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetSnrConfig(VpsHal_Handle handle,
                              const VpsHal_DeihSnrConfig *config,
                              Ptr configOvlyPtr)
{
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Check whether parameters are valid */
    GT_assert(VpsHalTrace,
        (config->gnmDevStableThrld <= VPSHAL_DEIH_MAX_GNM_DEV_STBL_THR));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Make register */
    tempReg = halDeihMakeSnrCfg0Reg(
                  regOvly->SNR_CFG0,
                  config->gnrChroma,
                  config->gnrLuma,
                  config->gnrAdaptive,
                  config->inrMode,
                  config->gnmDevStableThrld);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->SNR_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_SNR_CFG0];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihSetMdtMiscConfig
 *  \brief Sets the other DEIH MDT configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the other DEIH MDT registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtMiscConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihSetMdtMiscConfig(VpsHal_Handle handle,
                                  const VpsHal_DeihMdtMiscConfig *config,
                                  Ptr configOvlyPtr)
{
    VpsHal_DeihObj     *deihObj;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;
    UInt32             *virRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    deihObj = (VpsHal_DeihObj *) handle;
    regOvly = deihObj->baseAddress;

    /* Make register */
    tempReg = halDeihMakeMdtMiscCfg0Reg(
                  regOvly->MDT_CFG0,
                  config->adaptiveCoring,
                  config->fldComp,
                  config->edgeKd,
                  config->edgeVarThrld,
                  config->forceSlomoDiff,
                  config->forceF02Diff,
                  config->forceMv3D,
                  config->forceMv2D,
                  config->useDynGain,
                  config->enable3PixelFilt);
    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->MDT_CFG0 = tempReg;
    }
    else            /* Update in config overlay memory */
    {
        /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
        virRegOffset =
            (UInt32 *) configOvlyPtr +
            deihObj->virRegOffset[VPSHAL_DEIH_CONFIGOVLYIDX_MDT_CFG0];
        *virRegOffset = tempReg;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetConfig
 *  \brief Gets the entire DEIH configuration from the actual DEIH registers.
 *
 *  This function gets the entire DEIH configuration. This function
 *  gets the configuration of all the sub-modules in DEIH using other
 *  get configuration function calls.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetConfig(VpsHal_Handle handle, VpsHal_DeihConfig *config)
{
    Int32               retVal = VPS_EFAIL;
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get Height and Width */
    tempReg = regOvly->DEI_CFG0;
    config->width = ((tempReg & CSL_VPS_DEI_H_DEI_CFG0_WIDTH_MASK) >>
                          CSL_VPS_DEI_H_DEI_CFG0_WIDTH_SHIFT);
    config->height = ((tempReg & CSL_VPS_DEI_H_DEI_CFG0_HEIGHT_MASK) >>
                          CSL_VPS_DEI_H_DEI_CFG0_HEIGHT_SHIFT);

    /* Get other DEI_H configurations */
    tempReg = regOvly->DEI_CFG1;
    if (0 != (tempReg & CSL_VPS_DEI_H_DEI_CFG1_PROGRESSIVE_MASK))
    {
        config->scanFormat = VPS_SF_PROGRESSIVE;
    }
    else            /* Interlace mode */
    {
        config->scanFormat = VPS_SF_INTERLACED;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_DEI_CFG1_ENABLE_3D_MASK))
    {
        config->tempInpEnable = TRUE;
    }
    else            /* Temporal interpolation disabled */
    {
        config->tempInpEnable = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_DEI_CFG1_CHROMA_3D_ENABLE_MASK))
    {
        config->tempInpChromaEnable = TRUE;
    }
    else            /* Temporal interpolation for chroma disabled */
    {
        config->tempInpChromaEnable = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_DEI_CFG1_FID_POLARITY_MASK))
    {
        config->fidPol = VPS_FIDPOL_INVERT;
    }
    else            /* Field ID polarity normal */
    {
        config->fidPol = VPS_FIDPOL_NORMAL;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_DEI_CFG1_BYPASS_DEI_MASK))
    {
        config->bypass = TRUE;
    }
    else            /* Bypass disabled - Normal operation */
    {
        config->bypass = FALSE;
    }

    /* Call individual functions to get the configuration */
    retVal = VpsHal_deihGetMdtConfig(handle, &config->mdt);
    retVal += VpsHal_deihGetEdiConfig(handle, &config->edi);
    retVal += VpsHal_deihGetTnrConfig(handle, &config->tnr);
    retVal += VpsHal_deihGetFmdConfig(handle, &config->fmd);
    retVal += VpsHal_deihGetSnrConfig(handle, &config->snr);

    return (retVal);
}



/**
 *  VpsHal_deihGetMdtConfig
 *  \brief Gets DEIH MDT configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to MDT module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetMdtConfig(VpsHal_Handle handle,
                              VpsHal_DeihMdtConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get MDT configurations */
    tempReg = regOvly->MDT_CFG0;
    config->fldMode = (UInt32) ((tempReg &
                          CSL_VPS_DEI_H_MDT_CFG0_MDT_NUM_FIELD_MODE_MASK) >>
                          CSL_VPS_DEI_H_MDT_CFG0_MDT_NUM_FIELD_MODE_SHIFT);
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_SPATMAX_BYPASS_MASK))
    {
        config->spatMaxBypass = TRUE;
    }
    else            /* Spatial Max filter enabled */
    {
        config->spatMaxBypass = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_TEMPMAX_BYPASS_MASK))
    {
        config->tempMaxBypass = TRUE;
    }
    else            /* Temporal Max filter enabled */
    {
        config->tempMaxBypass = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_MVIIR_ENABLE_MASK))
    {
        config->mvIIR = TRUE;
    }
    else            /* Motion Vector IIR filtering disabled */
    {
        config->mvIIR = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_LC_MODE_MASK))
    {
        config->lcModeEnable = TRUE;
    }
    else            /* Low cost mode disabled */
    {
        config->lcModeEnable = FALSE;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetEdiConfig
 *  \brief Gets DEIH EDI configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to EDI module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihEdiConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetEdiConfig(VpsHal_Handle handle,
                              VpsHal_DeihEdiConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get EDI configurations */
    tempReg = regOvly->EDI_CFG0;
    config->inpMode = (UInt32) ((tempReg &
                            CSL_VPS_DEI_H_EDI_CFG0_EDI_INP_MODE_MASK) >>
                            CSL_VPS_DEI_H_EDI_CFG0_EDI_INP_MODE_SHIFT);
    if (0 != (tempReg & CSL_VPS_DEI_H_EDI_CFG0_EDI_CEI_ENABLE_MASK))
    {
        config->chromaEdiEnable = TRUE;
    }
    else            /* EDI for chroma disabled */
    {
        config->chromaEdiEnable = FALSE;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetTnrConfig
 *  \brief Gets DEIH TNR configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to TNR module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihTnrConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetTnrConfig(VpsHal_Handle handle,
                              VpsHal_DeihTnrConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get TNR configurations */
    tempReg = regOvly->TNR_CFG0;
    if (0 != (tempReg & VPSHAL_DEIH_TNR_CFG0_TNR_MODE_CHROMA_MASK))
    {
        config->chroma = TRUE;
    }
    else            /* TNR for Chroma disabled */
    {
        config->chroma = FALSE;
    }
    if (0 != (tempReg & VPSHAL_DEIH_TNR_CFG0_TNR_MODE_LUMA_MASK))
    {
        config->luma = TRUE;
    }
    else            /* TNR for Luma disabled */
    {
        config->luma = FALSE;
    }
    if (0 != (tempReg & VPSHAL_DEIH_TNR_CFG0_TNR_MODE_ADAPTIVE_MASK))
    {
        config->adaptive = TRUE;
    }
    else            /* Adaptive TNR disabled */
    {
        config->adaptive = FALSE;
    }
    config->advMode = (UInt32) ((tempReg &
                CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_MODE_MASK) >>
                CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_MODE_SHIFT);
    config->advScaleFactor = ((tempReg &
                CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_SCALE_FACTOR_MASK) >>
                CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_SCALE_FACTOR_SHIFT);
    if (0 != (tempReg & CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_DET_ENABLE_MASK))
    {
        config->skt = TRUE;
    }
    else            /* Skin tone detection disabled */
    {
        config->skt = FALSE;
    }
    config->maxSktGain = ((tempReg &
                CSL_VPS_DEI_H_TNR_CFG0_TNR_MAX_GAIN_SKINTONE_MASK) >>
                CSL_VPS_DEI_H_TNR_CFG0_TNR_MAX_GAIN_SKINTONE_SHIFT);
    config->minChromaSktThrld = ((tempReg &
                CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_MINC_THR_MASK) >>
                CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_MINC_THR_SHIFT);

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetFmdConfig
 *  \brief Gets DEIH FMD configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to FMD module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihFmdConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetFmdConfig(VpsHal_Handle handle,
                              VpsHal_DeihFmdConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get FMD configurations */
    tempReg = regOvly->FMD_CFG0;
    if (0 != (tempReg & CSL_VPS_DEI_H_FMD_CFG0_FMD_FMP_ENABLE_MASK))
    {
        config->filmMode = TRUE;
    }
    else            /* Film mode detection disabled */
    {
        config->filmMode = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_FMD_CFG0_FMD_BED_ENABLE_MASK))
    {
        config->bed = TRUE;
    }
    else            /* Bad edit detection disabled */
    {
        config->bed = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_FMD_CFG0_FMD_WINDOW_ENABLE_MASK))
    {
        config->window = TRUE;
    }
    else            /* FMD window disabled */
    {
        config->window = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_FMD_CFG0_FMD_LOCK_MASK))
    {
        config->lock = TRUE;
    }
    else            /* FMD Lock disabled */
    {
        config->lock = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_FMD_CFG0_FMD_JAM_DIR_MASK))
    {
        config->jamDir = VPS_DEI_FMDJAMDIR_NEXT_FLD;
    }
    else            /* Jam direction - Previous field */
    {
        config->jamDir = VPS_DEI_FMDJAMDIR_PREV_FLD;
    }

    /* Get FMD window configurations - lower limits */
    tempReg = regOvly->FMD_CFG2;
    config->windowMinx = ((tempReg &
                            CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINX_MASK) >>
                            CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINX_SHIFT);
    config->windowMiny = ((tempReg &
                            CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINY_MASK) >>
                            CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINY_SHIFT);

    /* Get FMD window configurations - upper limits */
    tempReg = regOvly->FMD_CFG3;
    config->windowMaxx = ((tempReg &
                            CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXX_MASK) >>
                            CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXX_SHIFT);
    config->windowMaxy = ((tempReg &
                            CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXY_MASK) >>
                            CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXY_SHIFT);

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetSnrConfig
 *  \brief Gets DEIH SNR configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to SNR module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihSnrConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetSnrConfig(VpsHal_Handle handle,
                              VpsHal_DeihSnrConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get SNR configurations */
    tempReg = regOvly->SNR_CFG0;
    if (0 != (tempReg & VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_CHROMA_MASK))
    {
        config->gnrChroma = TRUE;
    }
    else            /* GNR for Chroma disabled */
    {
        config->gnrChroma = FALSE;
    }
    if (0 != (tempReg & VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_LUMA_MASK))
    {
        config->gnrLuma = TRUE;
    }
    else            /* GNR for Luma disabled */
    {
        config->gnrLuma = FALSE;
    }
    if (0 != (tempReg & VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_ADAPTIVE_MASK))
    {
        config->gnrAdaptive = TRUE;
    }
    else            /* Adaptive GNR disabled */
    {
        config->gnrAdaptive = FALSE;
    }
    config->inrMode = (UInt32) ((tempReg &
                    CSL_VPS_DEI_H_SNR_CFG0_SNR_INR_MODE_MASK) >>
                    CSL_VPS_DEI_H_SNR_CFG0_SNR_INR_MODE_SHIFT);
    config->gnmDevStableThrld = ((tempReg &
                    CSL_VPS_DEI_H_SNR_CFG0_GNM_DEV_STABLE_THR_MASK) >>
                    CSL_VPS_DEI_H_SNR_CFG0_GNM_DEV_STABLE_THR_SHIFT);

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetMdtMiscConfig
 *  \brief Gets other DEIH MDT configuration from the actual DEIH registers.
 *
 *  This function gets the other DEIH configuration specific to MDT module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtMiscConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetMdtMiscConfig(VpsHal_Handle handle,
                                  VpsHal_DeihMdtMiscConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get MDT Misc configurations */
    tempReg = regOvly->MDT_CFG0;
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_ADAPTIVE_COR_ENABLE_MASK))
    {
        config->adaptiveCoring = TRUE;
    }
    else            /* Adaptive coring disabled */
    {
        config->adaptiveCoring = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_EN_FIELDCOMP_FLT_MASK))
    {
        config->fldComp = TRUE;
    }
    else            /* Field Comp disabled */
    {
        config->fldComp = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_EDGE_KD_MASK))
    {
        config->edgeKd = TRUE;
    }
    else            /* Edge KD disabled */
    {
        config->edgeKd = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_EDGE_VARTH_MASK))
    {
        config->edgeVarThrld = TRUE;
    }
    else            /* Edge Var Threshold disabled */
    {
        config->edgeVarThrld = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_FORCE_SLOMO_DIFF_MASK))
    {
        config->forceSlomoDiff = TRUE;
    }
    else            /* Force slomo difference disabled */
    {
        config->forceSlomoDiff = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_FORCE_F02_DIFF_MASK))
    {
        config->forceF02Diff = TRUE;
    }
    else            /* Force field 02 difference 2D disabled */
    {
        config->forceF02Diff = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_MV_FORCE_3D_MASK))
    {
        config->forceMv3D = TRUE;
    }
    else            /* Force Motion Vector 3D disabled */
    {
        config->forceMv3D = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_MV_FORCE_2D_MASK))
    {
        config->forceMv2D = TRUE;
    }
    else            /* Force Motion Vector 2D disabled */
    {
        config->forceMv2D = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_DYN_GAIN_MASK))
    {
        config->useDynGain = TRUE;
    }
    else            /* Dynamic gain disabled */
    {
        config->useDynGain = FALSE;
    }
    if (0 != (tempReg & CSL_VPS_DEI_H_MDT_CFG0_MDT_EN_PIX1X3FILT_MASK))
    {
        config->enable3PixelFilt = TRUE;
    }
    else            /* 3 Pixel filtering  disabled */
    {
        config->enable3PixelFilt = FALSE;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetFmdStatus
 *  \brief Returns DEIH FMD status information that is needed by the FMD
 *  software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param status           Pointer to VpsHal_DeihFmdStatus structure
 *                          containing the register status.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetFmdStatus(VpsHal_Handle handle,
                              VpsHal_DeihFmdStatus *status)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != status));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get FMD Frame Difference status */
    tempReg = regOvly->FMD_STAT0;
    status->frameDiff = ((tempReg &
                    CSL_VPS_DEI_H_FMD_STAT0_FMD_FRAME_DIFF_MASK) >>
                    CSL_VPS_DEI_H_FMD_STAT0_FMD_FRAME_DIFF_SHIFT);

    /* Get FMD Field Difference status */
    tempReg = regOvly->FMD_STAT1;
    status->fldDiff = ((tempReg &
                    CSL_VPS_DEI_H_FMD_STAT1_FMD_FIELD_DIFF_MASK) >>
                    CSL_VPS_DEI_H_FMD_STAT1_FMD_FIELD_DIFF_SHIFT);

    /* Get FMD CAF and Reset status */
    tempReg = regOvly->FMD_STAT2;
    status->caf = ((tempReg & CSL_VPS_DEI_H_FMD_STAT2_FMD_CAF_MASK) >>
                    CSL_VPS_DEI_H_FMD_STAT2_FMD_CAF_SHIFT);
    if (0 != (tempReg & CSL_VPS_DEI_H_FMD_STAT2_FMD_RESET_MASK))
    {
        status->reset = TRUE;
    }
    else            /* FMD reset disabled */
    {
        status->reset = FALSE;
    }

    return (VPS_SOK);
}



/**
 *  VpsHal_deihUpdateFmdConfig
 *  \brief Updates DEIH FMD configuration that needs to be updated by the
 *  FMD software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihFmdUpdateConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihUpdateFmdConfig(VpsHal_Handle handle,
                                 const VpsHal_DeihFmdUpdateConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Reset the bit fields which are being set */
    tempReg = regOvly->FMD_CFG0;
    tempReg &= ~(CSL_VPS_DEI_H_FMD_CFG0_FMD_LOCK_MASK |
                 CSL_VPS_DEI_H_FMD_CFG0_FMD_JAM_DIR_MASK);

    /* Set FMD configurations */
    if (TRUE == config->lock)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_LOCK_MASK;
    }
    if (VPS_DEI_FMDJAMDIR_NEXT_FLD == config->jamDir)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_JAM_DIR_MASK;
    }

    /* Write to actual register */
    regOvly->FMD_CFG0 = tempReg;

    return (VPS_SOK);
}



/**
 *  VpsHal_deihGetGnmStatus
 *  \brief Returns DEIH GNM status information that is needed by the GNM
 *  software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param status           Pointer to VpsHal_DeihGnmStatus structure
 *                          containing the register status.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihGetGnmStatus(VpsHal_Handle handle,
                              VpsHal_DeihGnmStatus *status)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != status));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Get GNM Y Hist Z status */
    tempReg = regOvly->GNM_STAT0;
    status->yHistz = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT0_GNM_Y_HIST_Z_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT0_GNM_Y_HIST_Z_SHIFT);
    /* Get GNM Y Hist 0 status */
    tempReg = regOvly->GNM_STAT1;
    status->yHist0 = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT1_GNM_Y_HIST_0_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT1_GNM_Y_HIST_0_SHIFT);
    /* Get GNM Y Hist 1 status */
    tempReg = regOvly->GNM_STAT2;
    status->yHist1 = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT2_GNM_Y_HIST_1_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT2_GNM_Y_HIST_1_SHIFT);
    /* Get GNM Y Hist m status */
    tempReg = regOvly->GNM_STAT3;
    status->yHistm = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT3_GNM_Y_HIST_M_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT3_GNM_Y_HIST_M_SHIFT);
    /* Get GNM C Hist Z status */
    tempReg = regOvly->GNM_STAT4;
    status->cHistz = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT4_GNM_C_HIST_Z_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT4_GNM_C_HIST_Z_SHIFT);
    /* Get GNM C Hist 0 status */
    tempReg = regOvly->GNM_STAT5;
    status->cHist0 = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT5_GNM_C_HIST_0_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT5_GNM_C_HIST_0_SHIFT);
    /* Get GNM C Hist 1 status */
    tempReg = regOvly->GNM_STAT6;
    status->cHist1 = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT6_GNM_C_HIST_1_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT6_GNM_C_HIST_1_SHIFT);
    /* Get GNM C Hist m status */
    tempReg = regOvly->GNM_STAT7;
    status->cHistm = ((tempReg &
                    CSL_VPS_DEI_H_GNM_STAT7_GNM_C_HIST_M_MASK) >>
                    CSL_VPS_DEI_H_GNM_STAT7_GNM_C_HIST_M_SHIFT);

    return (VPS_SOK);
}



/**
 *  VpsHal_deihUpdateGnmConfig
 *  \brief Updates DEIH GNM configuration that needs to be updated by the
 *  GNM software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihGnmUpdateConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns VPS_SOK on success else returns error value
 */
Int32 VpsHal_deihUpdateGnmConfig(VpsHal_Handle handle,
                                 const VpsHal_DeihGnmUpdateConfig *config)
{
    UInt32              tempReg;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = ((VpsHal_DeihObj *) handle)->baseAddress;

    /* Reset the bit fields which are being set */
    tempReg = regOvly->GNM_CFG0;
    tempReg &= ~(CSL_VPS_DEI_H_GNM_CFG0_GNM_Y_NOISE_DEV_MASK |
                 CSL_VPS_DEI_H_GNM_CFG0_GNM_C_NOISE_DEV_MASK);

    /* Set GNM CFG0 configurations */
    tempReg |= (((UInt32) config->yNoiseDev) <<
               CSL_VPS_DEI_H_GNM_CFG0_GNM_Y_NOISE_DEV_SHIFT);
    tempReg |= (((UInt32) config->cNoiseDev) <<
               CSL_VPS_DEI_H_GNM_CFG0_GNM_C_NOISE_DEV_SHIFT);

    /* Write to actual register */
    regOvly->GNM_CFG0 = tempReg;

    /* Reset the bit fields which are being set */
    tempReg = regOvly->GNM_CFG1;
    tempReg &= ~(CSL_VPS_DEI_H_GNM_CFG1_GNM_Y_DELTA_0_MASK |
                 CSL_VPS_DEI_H_GNM_CFG1_GNM_C_DELTA_0_MASK);

    /* Set GNM CFG1 configurations */
    tempReg |= (((UInt32) config->yDelta0) <<
               CSL_VPS_DEI_H_GNM_CFG1_GNM_Y_DELTA_0_SHIFT);
    tempReg |= (((UInt32) config->cDelta0) <<
               CSL_VPS_DEI_H_GNM_CFG1_GNM_C_DELTA_0_SHIFT);

    /* Write to actual register */
    regOvly->GNM_CFG1 = tempReg;

    /* Reset the bit fields which are being set */
    tempReg = regOvly->GNM_CFG2;
    tempReg &= ~(CSL_VPS_DEI_H_GNM_CFG2_GNM_Y_DELTA_1_MASK |
                 CSL_VPS_DEI_H_GNM_CFG2_GNM_C_DELTA_1_MASK);

    /* Set GNM CFG2 configurations */
    tempReg |= (((UInt32) config->yDelta1) <<
               CSL_VPS_DEI_H_GNM_CFG2_GNM_Y_DELTA_1_SHIFT);
    tempReg |= (((UInt32) config->cDelta1) <<
               CSL_VPS_DEI_H_GNM_CFG2_GNM_C_DELTA_1_SHIFT);

    /* Write to actual register */
    regOvly->GNM_CFG2 = tempReg;

    return (VPS_SOK);
}



/**
 *  halDeihInitInstance
 *  Initialize the DEIH instance by writing to expert registers and figuring
 *  out the virtual offsets of registers for config overlay memory.
 */
static Int32 halDeihInitInstance(VpsHal_DeihObj *deihObj)
{
    Int32                   retVal;
    CSL_VpsDeihRegsOvly     regOvly;
    VpsHal_DeihExpertConfig deihExpertConfig = VPSHAL_DEIH_DEFAULT_EXPERT_VAL;

    regOvly = deihObj->baseAddress;

    /* Set the expert registers with recommended value */
    retVal = halDeihSetExpertConfig(deihObj, &deihExpertConfig);

    /* Initialize the register offsets for the registers which are used
     * to form the VPDMA configuration overlay */
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_DEI_CFG0] =
                                        (UInt32 *) &regOvly->DEI_CFG0;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_DEI_CFG1] =
                                        (UInt32 *) &regOvly->DEI_CFG1;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_MDT_CFG0] =
                                        (UInt32 *) &regOvly->MDT_CFG0;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_EDI_CFG0] =
                                        (UInt32 *) &regOvly->EDI_CFG0;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_TNR_CFG0] =
                                        (UInt32 *) &regOvly->TNR_CFG0;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG0] =
                                        (UInt32 *) &regOvly->FMD_CFG0;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG2] =
                                        (UInt32 *) &regOvly->FMD_CFG2;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_FMD_CFG3] =
                                        (UInt32 *) &regOvly->FMD_CFG3;
    deihObj->regOffset[VPSHAL_DEIH_CONFIGOVLYIDX_SNR_CFG0] =
                                        (UInt32 *) &regOvly->SNR_CFG0;

    /* Get the size of the overlay for DEIH registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    deihObj->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                  deihObj->regOffset,
                                  VPSHAL_DEIH_NUM_REG_IDX,
                                  deihObj->virRegOffset);

    return (retVal);
}



/**
 *  halDeihSetExpertConfig
 *  Set the expert registers with expert values.
 */
static Int32 halDeihSetExpertConfig(VpsHal_DeihObj *deihObj,
                                    const VpsHal_DeihExpertConfig *config)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != deihObj));
    GT_assert(VpsHalTrace, (NULL != config));

    regOvly = deihObj->baseAddress;

    /* Write to MDT expert registers */
    regOvly->MDT_CFG0 = config->mdtCfg0;
    regOvly->MDT_CFG1 = config->mdtCfg1;
    regOvly->MDT_CFG2 = config->mdtCfg2;
    regOvly->MDT_CFG3 = config->mdtCfg3;
    regOvly->MDT_CFG4 = config->mdtCfg4;
    regOvly->MDT_CFG5 = config->mdtCfg5;
    regOvly->MDT_CFG6 = config->mdtCfg6;
    regOvly->MDT_CFG7 = config->mdtCfg7;
    regOvly->MDT_CFG8 = config->mdtCfg8;
    regOvly->MDT_CFG9 = config->mdtCfg9;

    /* Write to EDI expert registers */
    regOvly->EDI_CFG1 = config->ediCfg1;
    regOvly->EDI_CFG2 = config->ediCfg2;
    regOvly->EDI_CFG3 = config->ediCfg3;
    regOvly->EDI_CFG4 = config->ediCfg4;
    regOvly->EDI_CFG5 = config->ediCfg5;
    regOvly->EDI_CFG6 = config->ediCfg6;
    regOvly->EDI_CFG7 = config->ediCfg7;

    /* Write to TNR expert registers */
    regOvly->TNR_CFG1 = config->tnrCfg1;
    regOvly->TNR_CFG2 = config->tnrCfg2;
    regOvly->TNR_CFG3 = config->tnrCfg3;
    regOvly->TNR_CFG4 = config->tnrCfg4;
    regOvly->TNR_CFG5 = config->tnrCfg5;
    regOvly->TNR_CFG6 = config->tnrCfg6;

    /* Write to FMD expert registers */
    regOvly->FMD_CFG1 = config->fmdCfg1;

    /* Write to SNR expert registers */
    regOvly->SNR_CFG1 = config->snrCfg1;
    regOvly->SNR_CFG2 = config->snrCfg2;

    /* Write to GNM expert registers */
    regOvly->GNM_CFG0 = config->gnmCfg0;
    regOvly->GNM_CFG1 = config->gnmCfg1;
    regOvly->GNM_CFG2 = config->gnmCfg2;
    regOvly->GNM_CFG3 = config->gnmCfg3;

    return (VPS_SOK);
}



/**
 *  halDeihMakeDeiCfg0Reg
 *  Make DEI CFG0 register with given parameters.
 */
static inline UInt32 halDeihMakeDeiCfg0Reg(UInt32 initRegVal,
                                           UInt16 width,
                                           UInt16 height)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_DEI_CFG0_WIDTH_MASK |
                 CSL_VPS_DEI_H_DEI_CFG0_HEIGHT_MASK);

    /* Set Height and Width */
    tempReg |= (((UInt32) width) << CSL_VPS_DEI_H_DEI_CFG0_WIDTH_SHIFT);
    tempReg |= (((UInt32) height) << CSL_VPS_DEI_H_DEI_CFG0_HEIGHT_SHIFT);

    return (tempReg);
}



/**
 *  halDeihMakeDeiCfg1Reg
 *  Make DEI CFG1 register with given parameters.
 */
static inline UInt32 halDeihMakeDeiCfg1Reg(UInt32 initRegVal,
                                           UInt32 scanFormat,
                                           UInt32 tempInpEnable,
                                           UInt32 tempInpChromaEnable,
                                           UInt32 fidPol,
                                           UInt32 bypass)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_DEI_CFG1_PROGRESSIVE_MASK |
                 CSL_VPS_DEI_H_DEI_CFG1_ENABLE_3D_MASK |
                 CSL_VPS_DEI_H_DEI_CFG1_CHROMA_3D_ENABLE_MASK |
                 CSL_VPS_DEI_H_DEI_CFG1_FID_POLARITY_MASK |
                 CSL_VPS_DEI_H_DEI_CFG1_BYPASS_DEI_MASK);

    /* Set other DEI_H configurations */
    if (VPS_SF_PROGRESSIVE == scanFormat)
    {
        tempReg |= CSL_VPS_DEI_H_DEI_CFG1_PROGRESSIVE_MASK;
    }
    if (TRUE == tempInpEnable)
    {
        tempReg |= CSL_VPS_DEI_H_DEI_CFG1_ENABLE_3D_MASK;
    }
    if (TRUE == tempInpChromaEnable)
    {
        tempReg |= CSL_VPS_DEI_H_DEI_CFG1_CHROMA_3D_ENABLE_MASK;
    }
    if (VPS_FIDPOL_INVERT == fidPol)
    {
        tempReg |= CSL_VPS_DEI_H_DEI_CFG1_FID_POLARITY_MASK;
    }
    if (TRUE == bypass)
    {
        tempReg |= CSL_VPS_DEI_H_DEI_CFG1_BYPASS_DEI_MASK;
    }

    return (tempReg);
}



/**
 *  halDeihMakeMdtCfg0Reg
 *  Make MDT CFG0 register with given parameters.
 */
static inline UInt32 halDeihMakeMdtCfg0Reg(UInt32 initRegVal,
                                           UInt32 fldMode,
                                           UInt32 spatMaxBypass,
                                           UInt32 tempMaxBypass,
                                           UInt32 mvIIR,
                                           UInt32 lcModeEnable)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_MDT_CFG0_MDT_NUM_FIELD_MODE_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_SPATMAX_BYPASS_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_TEMPMAX_BYPASS_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_MVIIR_ENABLE_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_LC_MODE_MASK);

    /* Set MDT configurations */
    tempReg |= (((UInt32) fldMode) <<
                   CSL_VPS_DEI_H_MDT_CFG0_MDT_NUM_FIELD_MODE_SHIFT);
    if (TRUE == spatMaxBypass)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_SPATMAX_BYPASS_MASK;
    }
    if (TRUE == tempMaxBypass)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_TEMPMAX_BYPASS_MASK;
    }
    if (TRUE == mvIIR)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_MVIIR_ENABLE_MASK;
    }
    if (TRUE == lcModeEnable)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_LC_MODE_MASK;
    }

    return (tempReg);
}



/**
 *  halDeihMakeEdiCfg0Reg
 *  Make EDI CFG0 register with given parameters.
 */
static inline UInt32 halDeihMakeEdiCfg0Reg(UInt32 initRegVal,
                                           UInt32 inpMode,
                                           UInt32 chromaEdiEnable)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_EDI_CFG0_EDI_INP_MODE_MASK |
                 CSL_VPS_DEI_H_EDI_CFG0_EDI_CEI_ENABLE_MASK);

    /* Set EDI configurations */
    tempReg |= (((UInt32) inpMode) <<
                   CSL_VPS_DEI_H_EDI_CFG0_EDI_INP_MODE_SHIFT);
    if (TRUE == chromaEdiEnable)
    {
        tempReg |= CSL_VPS_DEI_H_EDI_CFG0_EDI_CEI_ENABLE_MASK;
    }

    return (tempReg);
}



/**
 *  halDeihMakeTnrCfg0Reg
 *  Make TNR CFG0 register with given parameters.
 */
static inline UInt32 halDeihMakeTnrCfg0Reg(UInt32 initRegVal,
                                           UInt32 chroma,
                                           UInt32 luma,
                                           UInt32 adaptive,
                                           UInt32 advMode,
                                           UInt8 advScaleFactor,
                                           UInt32 skt,
                                           UInt8 maxSktGain,
                                           UInt8 minChromaSktThrld)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_TNR_CFG0_TNR_MODE_MASK |
                 CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_MODE_MASK |
                 CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_SCALE_FACTOR_MASK |
                 CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_DET_ENABLE_MASK |
                 CSL_VPS_DEI_H_TNR_CFG0_TNR_MAX_GAIN_SKINTONE_MASK |
                 CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_MINC_THR_MASK);

    /* Set TNR configurations */
    if (TRUE == chroma)
    {
        tempReg |= VPSHAL_DEIH_TNR_CFG0_TNR_MODE_CHROMA_MASK;
    }
    if (TRUE == luma)
    {
        tempReg |= VPSHAL_DEIH_TNR_CFG0_TNR_MODE_LUMA_MASK;
    }
    if (TRUE == adaptive)
    {
        tempReg |= VPSHAL_DEIH_TNR_CFG0_TNR_MODE_ADAPTIVE_MASK;
    }
    tempReg |= (((UInt32) advMode) <<
                    CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_MODE_SHIFT);
    tempReg |= (((UInt32) advScaleFactor) <<
                CSL_VPS_DEI_H_TNR_CFG0_TNR_ADVANCED_SCALE_FACTOR_SHIFT);
    if (TRUE == skt)
    {
        tempReg |= CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_DET_ENABLE_MASK;
    }
    tempReg |= (((UInt32) maxSktGain) <<
                   CSL_VPS_DEI_H_TNR_CFG0_TNR_MAX_GAIN_SKINTONE_SHIFT);
    tempReg |= (((UInt32) minChromaSktThrld) <<
                   CSL_VPS_DEI_H_TNR_CFG0_TNR_SKINTONE_MINC_THR_SHIFT);

    return (tempReg);
}



/**
 *  halDeihMakeFmdCfg0Reg
 *  Make FMD CFG0 register with given parameters.
 */
static inline UInt32 halDeihMakeFmdCfg0Reg(UInt32 initRegVal,
                                           UInt32 filmMode,
                                           UInt32 bed,
                                           UInt32 window,
                                           UInt32 lock,
                                           UInt32 jamDir)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_FMD_CFG0_FMD_FMP_ENABLE_MASK |
                 CSL_VPS_DEI_H_FMD_CFG0_FMD_BED_ENABLE_MASK |
                 CSL_VPS_DEI_H_FMD_CFG0_FMD_WINDOW_ENABLE_MASK |
                 CSL_VPS_DEI_H_FMD_CFG0_FMD_LOCK_MASK |
                 CSL_VPS_DEI_H_FMD_CFG0_FMD_JAM_DIR_MASK);

    /* Set FMD configurations */
    if (TRUE == filmMode)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_FMP_ENABLE_MASK;
    }
    if (TRUE == bed)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_BED_ENABLE_MASK;
    }
    if (TRUE == window)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_WINDOW_ENABLE_MASK;
    }
    if (TRUE == lock)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_LOCK_MASK;
    }
    if (VPS_DEI_FMDJAMDIR_NEXT_FLD == jamDir)
    {
        tempReg |= CSL_VPS_DEI_H_FMD_CFG0_FMD_JAM_DIR_MASK;
    }

    return (tempReg);
}



/**
 *  halDeihMakeFmdCfg2Reg
 *  Make FMD CFG2 register with given parameters.
 */
static inline UInt32 halDeihMakeFmdCfg2Reg(UInt32 initRegVal,
                                           UInt16 windowMinx,
                                           UInt16 windowMiny)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINX_MASK |
                 CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINY_MASK);

    /* Set FMD window configurations - lower limits */
    tempReg |= (((UInt32) windowMinx) <<
                   CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINX_SHIFT);
    tempReg |= (((UInt32) windowMiny) <<
                   CSL_VPS_DEI_H_FMD_CFG2_FMD_WINDOW_MINY_SHIFT);

    return (tempReg);
}



/**
 *  halDeihMakeFmdCfg3Reg
 *  Make FMD CFG3 register with given parameters.
 */
static inline UInt32 halDeihMakeFmdCfg3Reg(UInt32 initRegVal,
                                           UInt16 windowMaxx,
                                           UInt16 windowMaxy)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXX_MASK |
                 CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXY_MASK);

    /* Set FMD window configurations - uppper limits */
    tempReg |= (((UInt32) windowMaxx) <<
                   CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXX_SHIFT);
    tempReg |= (((UInt32) windowMaxy) <<
                   CSL_VPS_DEI_H_FMD_CFG3_FMD_WINDOW_MAXY_SHIFT);

    return (tempReg);
}



/**
 *  halDeihMakeSnrCfg0Reg
 *  Make SNR CFG0 register with given parameters.
 */
static inline UInt32 halDeihMakeSnrCfg0Reg(UInt32 initRegVal,
                                           UInt32 gnrChroma,
                                           UInt32 gnrLuma,
                                           UInt32 gnrAdaptive,
                                           UInt32 inrMode,
                                           UInt8 gnmDevStableThrld)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_SNR_CFG0_SNR_GNR_MODE_MASK |
                 CSL_VPS_DEI_H_SNR_CFG0_SNR_INR_MODE_MASK |
                 CSL_VPS_DEI_H_SNR_CFG0_GNM_DEV_STABLE_THR_MASK);

    /* Set SNR configurations */
    if (TRUE == gnrChroma)
    {
        tempReg |= VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_CHROMA_MASK;
    }
    if (TRUE == gnrLuma)
    {
        tempReg |= VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_LUMA_MASK;
    }
    if (TRUE == gnrAdaptive)
    {
        tempReg |= VPSHAL_DEIH_SNR_CFG0_SNR_GNR_MODE_ADAPTIVE_MASK;
    }
    tempReg |= (((UInt32) inrMode) <<
                   CSL_VPS_DEI_H_SNR_CFG0_SNR_INR_MODE_SHIFT);
    tempReg |= (((UInt32) gnmDevStableThrld) <<
                   CSL_VPS_DEI_H_SNR_CFG0_GNM_DEV_STABLE_THR_SHIFT);

    return (tempReg);
}



/**
 *  halDeihMakeMdtMiscCfg0Reg
 *  Make MDT CFG0 register with given misc parameters.
 */
static inline UInt32 halDeihMakeMdtMiscCfg0Reg(UInt32 initRegVal,
                                               UInt32 adaptiveCoring,
                                               UInt32 fldComp,
                                               UInt32 edgeKd,
                                               UInt32 edgeVarThrld,
                                               UInt32 forceSlomoDiff,
                                               UInt32 forceF02Diff,
                                               UInt32 forceMv3D,
                                               UInt32 forceMv2D,
                                               UInt32 useDynGain,
                                               UInt32 enable3PixelFilt)
{
    UInt32  tempReg;

    /* Reset the bit fields which are being set */
    tempReg = initRegVal;
    tempReg &= ~(CSL_VPS_DEI_H_MDT_CFG0_MDT_ADAPTIVE_COR_ENABLE_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_EN_FIELDCOMP_FLT_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_EDGE_KD_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_EDGE_VARTH_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_FORCE_SLOMO_DIFF_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_FORCE_F02_DIFF_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_MV_FORCE_3D_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_MV_FORCE_2D_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_DYN_GAIN_MASK |
                 CSL_VPS_DEI_H_MDT_CFG0_MDT_EN_PIX1X3FILT_MASK);

    /* Set MDT Misc configurations */
    if (TRUE == adaptiveCoring)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_ADAPTIVE_COR_ENABLE_MASK;
    }
    if (TRUE == fldComp)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_EN_FIELDCOMP_FLT_MASK;
    }
    if (TRUE == edgeKd)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_EDGE_KD_MASK;
    }
    if (TRUE == edgeVarThrld)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_EDGE_VARTH_MASK;
    }
    if (TRUE == forceSlomoDiff)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_FORCE_SLOMO_DIFF_MASK;
    }
    if (TRUE == forceF02Diff)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_FORCE_F02_DIFF_MASK;
    }
    if (TRUE == forceMv3D)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_MV_FORCE_3D_MASK;
    }
    if (TRUE == forceMv2D)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_MV_FORCE_2D_MASK;
    }
    if (TRUE == useDynGain)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_USE_DYN_GAIN_MASK;
    }
    if (TRUE == enable3PixelFilt)
    {
        tempReg |= CSL_VPS_DEI_H_MDT_CFG0_MDT_EN_PIX1X3FILT_MASK;
    }

    return (tempReg);
}


/* Functions to write advanced configuration */
Int32 VpsHal_deiHqSetAdvConfig(VpsHal_Handle handle,
                               const Vps_DeiHqRdWrAdvCfg *advCfg,
                               Ptr configOvlyPtr)
{
    Int32 retVal = VPS_SOK;

    retVal |= vpsDeihSetAdvMdtCfg(handle, &advCfg->mdtCfg);
    retVal |= vpsDeihSetAdvEdiCfg(handle, &advCfg->ediCfg);
    retVal |= vpsDeihSetAdvTnrCfg(handle, &advCfg->tnrCfg);
    retVal |= vpsDeihSetAdvFmdCfg(handle, &advCfg->fmdCfg);
    retVal |= vpsDeihSetAdvSnrCfg(handle, &advCfg->snrCfg);
    retVal |= vpsDeihSetAdvGnmCfg(handle, &advCfg->gnmCfg);

    return (retVal);
}



/* Functions to read advanced configuration */
Int32 VpsHal_deiHqGetAdvConfig(VpsHal_Handle handle,
                               Vps_DeiHqRdWrAdvCfg *advCfg)
{
    Int32 retVal = VPS_SOK;

    retVal |= vpsDeihGetAdvMdtCfg(handle, &advCfg->mdtCfg);
    retVal |= vpsDeihGetAdvEdiCfg(handle, &advCfg->ediCfg);
    retVal |= vpsDeihGetAdvTnrCfg(handle, &advCfg->tnrCfg);
    retVal |= vpsDeihGetAdvFmdCfg(handle, &advCfg->fmdCfg);
    retVal |= vpsDeihGetAdvSnrCfg(handle, &advCfg->snrCfg);
    retVal |= vpsDeihGetAdvGnmCfg(handle, &advCfg->gnmCfg);

    return (retVal);
}



/* Functions to read/write advanced configuration */
static Int32 vpsDeihSetAdvMdtCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqMdtConfig *mdtCfg)
{
    UInt32              regValue;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != mdtCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* MDT_CFG1 */
    regValue = 0;
    regValue |= (mdtCfg->mvCorThrMax <<
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MAX_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MAX_MASK;
    regValue |= (mdtCfg->mvCorThr <<
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MASK;
    regValue |= (mdtCfg->mvCorThr <<
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MASK;
    regValue |= (mdtCfg->sfCorThr[0] <<
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR1_MASK;
    regValue |= (mdtCfg->sfCorThr[1] <<
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR2_MASK;
    regValue |= (mdtCfg->sfCorThr[2] <<
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR3_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR3_MASK;
    regOvly->MDT_CFG1 = regValue;

    /* MDT_CFG2 */
    regValue = 0;
    regValue |= (mdtCfg->sceneChangeThr <<
        CSL_VPS_DEI_H_MDT_CFG2_MDT_SCENE_CHANGE_THR_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_SCENE_CHANGE_THR_MASK;
    regValue |= (mdtCfg->mvstmCorThr <<
        CSL_VPS_DEI_H_MDT_CFG2_MDT_MVSTMAX_COR_THR_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_MVSTMAX_COR_THR_MASK;
    regValue |= (mdtCfg->stfFdThr[0] <<
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR1_MASK;
    regValue |= (mdtCfg->stfFdThr[1] <<
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR2_MASK;
    regValue |= (mdtCfg->stfFdThr[2] <<
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR3_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR3_MASK;
    regOvly->MDT_CFG2 = regValue;

    /* MDT_CFG3 */
    regValue = 0;
    regValue |= (mdtCfg->edgeDetectThr <<
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_TH_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_TH_MASK;
    regValue |= (mdtCfg->activityThr <<
        CSL_VPS_DEI_H_MDT_CFG3_MDT_ACT_TH_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_ACT_TH_MASK;
    regValue |= (mdtCfg->edgeRtShift <<
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_RTSHIFT_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_RTSHIFT_MASK;
    regValue |= (mdtCfg->vfGain <<
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_GAIN_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_GAIN_MASK;
    regValue |= (mdtCfg->vfThr <<
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_TH_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_TH_MASK;
    regOvly->MDT_CFG3 = regValue;

    /* MDT_CFG4 */
    regValue = 0;
    regValue |= (mdtCfg->thrVarLimit <<
        CSL_VPS_DEI_H_MDT_CFG4_MDT_TH1_VAR_LT_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_TH1_VAR_LT_MASK;
    regValue |= (mdtCfg->kdetVarThrGain <<
        CSL_VPS_DEI_H_MDT_CFG4_MDT_KDET_VAR_TH1_GA_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_KDET_VAR_TH1_GA_MASK;
    regValue |= (mdtCfg->edgeRtShiftVarth <<
        CSL_VPS_DEI_H_MDT_CFG4_MDT_EDG_RTSHIFT_VARTH_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_EDG_RTSHIFT_VARTH_MASK;
    regValue |= (mdtCfg->yDetectThr <<
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_TH_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_TH_MASK;
    regValue |= (mdtCfg->yDetectGain <<
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_GA_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_GA_MASK;
    regOvly->MDT_CFG4 = regValue;

    /* MDT_CFG5 */
    regValue = 0;
    regValue |= (mdtCfg->actDetectThr[0] <<
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH1_MASK;
    regValue |= (mdtCfg->actDetectThr[1] <<
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH2_MASK;
    regValue |= (mdtCfg->actDetectThr[2] <<
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH3_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH3_MASK;
    regOvly->MDT_CFG5 = regValue;

    /* MDT_CFG6 */
    regValue = 0;
    regValue |= (mdtCfg->fld20Thr1 <<
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH1_MASK;
    regValue |= (mdtCfg->fld20Thr2 <<
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH2_MASK;
    regValue |= (mdtCfg->fld20Gain1 <<
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA1_MASK;
    regValue |= (mdtCfg->fld20Gain2 <<
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA2_MASK;
    regOvly->MDT_CFG6 = regValue;

    /* MDT_CFG7 */
    regValue = 0;
    regValue |= (mdtCfg->fikmRtShift[0u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT0_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT0_MASK;
    regValue |= (mdtCfg->fikmRtShift[1u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT1_MASK;
    regValue |= (mdtCfg->fikmRtShift[2u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT2_MASK;
    regValue |= (mdtCfg->fikmRtShift[3u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT3_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT3_MASK;
    regValue |= (mdtCfg->fikmRtShift[4u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT4_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT4_MASK;
    regValue |= (mdtCfg->fikmRtShift[5u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT5_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT5_MASK;
    regValue |= (mdtCfg->fikmRtShift[6u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT6_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT6_MASK;
    regValue |= (mdtCfg->fikmRtShift[7u] <<
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT7_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT7_MASK;
    regOvly->MDT_CFG7 = regValue;

    /* MDT_CFG8 */
    regValue = (mdtCfg->pixFltHfCore <<
        CSL_VPS_DEI_H_MDT_CFG8_MDT_PIXFLT_HF_CORE_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG8_MDT_PIXFLT_HF_CORE_MASK;
    regOvly->MDT_CFG8 = regValue;

    /* MDT_CFG9 */
    regValue = 0;
    regValue |= (mdtCfg->fld012Thr1 <<
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH1_MASK;
    regValue |= (mdtCfg->fld012Thr2 <<
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH2_MASK;
    regValue |= (mdtCfg->fld012Gain1 <<
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA1_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA1_MASK;
    regValue |= (mdtCfg->fld012Gain2 <<
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA2_SHIFT) &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA2_MASK;
    regOvly->MDT_CFG9 = regValue;

    return (VPS_SOK);
}



/* Functions to read advanced MDT configuration */
static Int32 vpsDeihGetAdvMdtCfg(VpsHal_Handle handle,
                                 Vps_DeiHqMdtConfig *mdtCfg)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != mdtCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* MDT_CFG1 */
    mdtCfg->mvCorThrMax = (regOvly->MDT_CFG1 &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MAX_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MAX_SHIFT;
    mdtCfg->mvCorThr = (regOvly->MDT_CFG1 &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG1_MDT_MV_COR_THR_SHIFT;
    mdtCfg->sfCorThr[0] = (regOvly->MDT_CFG1 &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR1_SHIFT;
    mdtCfg->sfCorThr[1] = (regOvly->MDT_CFG1 &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR2_SHIFT;
    mdtCfg->sfCorThr[2] = (regOvly->MDT_CFG1 &
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR3_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG1_MDT_SF_SC_THR3_SHIFT;

    /* MDT_CFG2 */
    mdtCfg->sceneChangeThr = (regOvly->MDT_CFG2 &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_SCENE_CHANGE_THR_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG2_MDT_SCENE_CHANGE_THR_SHIFT;
    mdtCfg->mvstmCorThr = (regOvly->MDT_CFG2 &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_MVSTMAX_COR_THR_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG2_MDT_MVSTMAX_COR_THR_SHIFT;
    mdtCfg->stfFdThr[0] = (regOvly->MDT_CFG2 &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR1_SHIFT;
    mdtCfg->stfFdThr[1] = (regOvly->MDT_CFG2 &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR2_SHIFT;
    mdtCfg->stfFdThr[2] = (regOvly->MDT_CFG2 &
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR3_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG2_MDT_STF_FD_THR3_SHIFT;

    /* MDT_CFG3 */
    mdtCfg->edgeDetectThr = (regOvly->MDT_CFG3 &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_TH_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_TH_SHIFT;
    mdtCfg->activityThr = (regOvly->MDT_CFG3 &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_ACT_TH_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG3_MDT_ACT_TH_SHIFT;
    mdtCfg->edgeRtShift = (regOvly->MDT_CFG3 &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_RTSHIFT_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG3_MDT_EDG_RTSHIFT_SHIFT;
    mdtCfg->vfGain= (regOvly->MDT_CFG3 &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_GAIN_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_GAIN_SHIFT;
    mdtCfg->vfThr = (regOvly->MDT_CFG3 &
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_TH_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG3_MDT_VF_TH_SHIFT;

    /* MDT_CFG4 */
    mdtCfg->thrVarLimit = (regOvly->MDT_CFG4 &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_TH1_VAR_LT_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG4_MDT_TH1_VAR_LT_SHIFT;
    mdtCfg->kdetVarThrGain = (regOvly->MDT_CFG4 &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_KDET_VAR_TH1_GA_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG4_MDT_KDET_VAR_TH1_GA_SHIFT;
    mdtCfg->edgeRtShiftVarth = (regOvly->MDT_CFG4 &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_EDG_RTSHIFT_VARTH_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG4_MDT_EDG_RTSHIFT_VARTH_SHIFT;
    mdtCfg->yDetectThr = (regOvly->MDT_CFG4 &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_TH_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_TH_SHIFT;
    mdtCfg->yDetectGain = (regOvly->MDT_CFG4 &
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_GA_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG4_MDT_Y_DET_GA_SHIFT;

    /* MDT_CFG5 */
    mdtCfg->actDetectThr[0] = (regOvly->MDT_CFG5 &
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH1_SHIFT;
    mdtCfg->actDetectThr[1] = (regOvly->MDT_CFG5 &
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH2_SHIFT;
    mdtCfg->actDetectThr[2] = (regOvly->MDT_CFG5 &
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH3_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG5_MDT_ACT_DET_TH3_SHIFT;

    /* MDT_CFG6 */
    mdtCfg->fld20Thr1 = (regOvly->MDT_CFG6 &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH1_SHIFT;
    mdtCfg->fld20Thr2 = (regOvly->MDT_CFG6 &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_TH2_SHIFT;
    mdtCfg->fld20Gain1 = (regOvly->MDT_CFG6 &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA1_SHIFT;
    mdtCfg->fld20Gain2 = (regOvly->MDT_CFG6 &
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG6_MDT_F20_GA2_SHIFT;

    /* MDT_CFG7 */
    mdtCfg->fikmRtShift[0u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT0_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT0_SHIFT;
    mdtCfg->fikmRtShift[1u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT1_SHIFT;
    mdtCfg->fikmRtShift[2u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT2_SHIFT;
    mdtCfg->fikmRtShift[3u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT3_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT3_SHIFT;
    mdtCfg->fikmRtShift[4u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT4_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT4_SHIFT;
    mdtCfg->fikmRtShift[5u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT5_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT5_SHIFT;
    mdtCfg->fikmRtShift[6u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT6_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT6_SHIFT;
    mdtCfg->fikmRtShift[7u] = (regOvly->MDT_CFG7 &
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT7_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG7_MDT_FIKM_RTSHIFT7_SHIFT;

    /* MDT_CFG8 */
    mdtCfg->pixFltHfCore = (regOvly->MDT_CFG8 &
        CSL_VPS_DEI_H_MDT_CFG8_MDT_PIXFLT_HF_CORE_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG8_MDT_PIXFLT_HF_CORE_SHIFT;

    /* MDT_CFG9 */
    mdtCfg->fld012Thr1 = (regOvly->MDT_CFG9 &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH1_SHIFT;
    mdtCfg->fld012Thr2 = (regOvly->MDT_CFG9 &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_TH2_SHIFT;
    mdtCfg->fld012Gain1 = (regOvly->MDT_CFG9 &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA1_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA1_SHIFT;
    mdtCfg->fld012Gain2 = (regOvly->MDT_CFG9 &
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA2_MASK) >>
        CSL_VPS_DEI_H_MDT_CFG9_MDT_F012_GA2_SHIFT;

    return (VPS_SOK);
}



/* Functions to Write advanced EDI configuration */
static Int32 vpsDeihSetAdvEdiCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqEdiConfig *ediCfg)
{
    UInt32              regValue;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != ediCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* EDI_CFG1 */
    regValue = 0u;
    regValue |= (ediCfg->flagAreaThr <<
        CSL_VPS_DEI_H_EDI_CFG1_EDI_FLAT_AREA_THR_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_FLAT_AREA_THR_MASK;
    regValue |= (ediCfg->spaIntThr <<
        CSL_VPS_DEI_H_EDI_CFG1_EDI_SPA_INT_THR_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_SPA_INT_THR_MASK;
    regValue |= (ediCfg->chroma3DCorThr <<
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CHROMA3D_COR_THR_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CHROMA3D_COR_THR_MASK;
    regValue |= (ediCfg->ceiThr <<
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CEI_THR_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CEI_THR_MASK;
    regOvly->EDI_CFG1 = regValue;

    /* EDI_CFG2 */
    regValue = 0u;
    regValue |= (ediCfg->sadThr[0u] <<
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR1_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR1_MASK;
    regValue |= (ediCfg->sadThr[5u] <<
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR6_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR6_MASK;
    regOvly->EDI_CFG2 = regValue;

    /* EDI_CFG3 */
    regValue = 0u;
    regValue |= (ediCfg->sadThr[1u] <<
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR2_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR2_MASK;
    regValue |= (ediCfg->sadThr[2u] <<
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR3_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR3_MASK;
    regValue |= (ediCfg->sadThr[3u] <<
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR4_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR4_MASK;
    regValue |= (ediCfg->sadThr[4u] <<
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR5_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR5_MASK;
    regOvly->EDI_CFG3 = regValue;

    /* EDI_CFG4 */
    regValue = 0u;
    regValue |= (ediCfg->lut[0u] <<
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT0_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT0_MASK;
    regValue |= (ediCfg->lut[1u] <<
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT1_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT1_MASK;
    regValue |= (ediCfg->lut[2u] <<
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT2_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT2_MASK;
    regValue |= (ediCfg->lut[3u] <<
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT3_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT3_MASK;
    regOvly->EDI_CFG4 = regValue;

    /* EDI_CFG5 */
    regValue = 0u;
    regValue |= (ediCfg->lut[4u] <<
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT4_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT4_MASK;
    regValue |= (ediCfg->lut[5u] <<
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT5_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT5_MASK;
    regValue |= (ediCfg->lut[6u] <<
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT6_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT6_MASK;
    regValue |= (ediCfg->lut[7u] <<
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT7_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT7_MASK;
    regOvly->EDI_CFG5 = regValue;

    /* EDI_CFG6 */
    regValue = 0u;
    regValue |= (ediCfg->lut[8u] <<
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT8_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT8_MASK;
    regValue |= (ediCfg->lut[9u] <<
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT9_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT9_MASK;
    regValue |= (ediCfg->lut[10u] <<
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT10_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT10_MASK;
    regValue |= (ediCfg->lut[11u] <<
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT11_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT11_MASK;
    regOvly->EDI_CFG6 = regValue;

    /* EDI_CFG7 */
    regValue = 0u;
    regValue |= (ediCfg->lut[12u] <<
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT12_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT12_MASK;
    regValue |= (ediCfg->lut[13u] <<
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT13_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT13_MASK;
    regValue |= (ediCfg->lut[14u] <<
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT14_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT14_MASK;
    regValue |= (ediCfg->lut[15u] <<
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT15_SHIFT) &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT15_MASK;
    regOvly->EDI_CFG7 = regValue;

    return (VPS_SOK);
}



/* Functions to read advanced EDI configuration */
static Int32 vpsDeihGetAdvEdiCfg(VpsHal_Handle handle,
                                 Vps_DeiHqEdiConfig *ediCfg)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != ediCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* EDI_CFG1 */
    ediCfg->flagAreaThr = (regOvly->EDI_CFG1 &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_FLAT_AREA_THR_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG1_EDI_FLAT_AREA_THR_SHIFT;
    ediCfg->spaIntThr = (regOvly->EDI_CFG1 &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_SPA_INT_THR_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG1_EDI_SPA_INT_THR_SHIFT;
    ediCfg->chroma3DCorThr = (regOvly->EDI_CFG1 &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CHROMA3D_COR_THR_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CHROMA3D_COR_THR_SHIFT;
    ediCfg->ceiThr = (regOvly->EDI_CFG1 &
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CEI_THR_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG1_EDI_CEI_THR_SHIFT;

    /* EDI_CFG2 */
    ediCfg->sadThr[0u] = (regOvly->EDI_CFG2 &
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR1_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR1_SHIFT;
    ediCfg->sadThr[5u] = (regOvly->EDI_CFG2 &
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR6_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG2_EDI_DIFF_THR6_SHIFT;

    /* EDI_CFG3 */
    ediCfg->sadThr[1u] = (regOvly->EDI_CFG3 &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR2_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR2_SHIFT;
    ediCfg->sadThr[2u] = (regOvly->EDI_CFG3 &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR3_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR3_SHIFT;
    ediCfg->sadThr[3u] = (regOvly->EDI_CFG3 &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR4_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR4_SHIFT;
    ediCfg->sadThr[4u] = (regOvly->EDI_CFG3 &
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR5_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG3_EDI_DIFF_THR5_SHIFT;

    /* EDI_CFG4 */
    ediCfg->lut[0u] = (regOvly->EDI_CFG4 &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT0_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT0_SHIFT;
    ediCfg->lut[1u] = (regOvly->EDI_CFG4 &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT1_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT1_SHIFT;
    ediCfg->lut[2u] = (regOvly->EDI_CFG4 &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT2_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT2_SHIFT;
    ediCfg->lut[3u] = (regOvly->EDI_CFG4 &
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT3_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG4_EDI_LUT3_SHIFT;

    /* EDI_CFG5 */
    ediCfg->lut[4u] = (regOvly->EDI_CFG5 &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT4_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT4_SHIFT;
    ediCfg->lut[5u] = (regOvly->EDI_CFG5 &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT5_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT5_SHIFT;
    ediCfg->lut[6u] = (regOvly->EDI_CFG5 &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT6_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT6_SHIFT;
    ediCfg->lut[7u] = (regOvly->EDI_CFG5 &
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT7_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG5_EDI_LUT7_SHIFT;

    /* EDI_CFG6 */
    ediCfg->lut[8u] = (regOvly->EDI_CFG6 &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT8_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT8_SHIFT;
    ediCfg->lut[9u] = (regOvly->EDI_CFG6 &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT9_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT9_SHIFT;
    ediCfg->lut[10u] = (regOvly->EDI_CFG6 &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT10_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT10_SHIFT;
    ediCfg->lut[11u] = (regOvly->EDI_CFG6 &
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT11_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG6_EDI_LUT11_SHIFT;

    /* EDI_CFG7 */
    ediCfg->lut[12u] = (regOvly->EDI_CFG7 &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT12_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT12_SHIFT;
    ediCfg->lut[13u] = (regOvly->EDI_CFG7 &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT13_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT13_SHIFT;
    ediCfg->lut[14u] = (regOvly->EDI_CFG7 &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT14_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT14_SHIFT;
    ediCfg->lut[15u] = (regOvly->EDI_CFG7 &
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT15_MASK) >>
        CSL_VPS_DEI_H_EDI_CFG7_EDI_LUT15_SHIFT;

    return (VPS_SOK);
}



/* Functions to Write advanced TNR configuration */
static Int32 vpsDeihSetAdvTnrCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqTnrConfig *tnrCfg)
{
    UInt32              regValue;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != tnrCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* TNR_CFG1 */
    regValue = 0u;
    regValue |= (tnrCfg->lumaMvLowThr <<
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_MASK;
    regValue |= (tnrCfg->chromaMvLowThr <<
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_MASK;
    regValue |= (tnrCfg->lumaMvLowThrMax <<
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_MAX_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_MAX_MASK;
    regValue |= (tnrCfg->chromaMvLowThrMax <<
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_MAX_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_MAX_MASK;
    regOvly->TNR_CFG1 = regValue;

    /* TNR_CFG2 */
    regValue = 0u;
    regValue |= (tnrCfg->lumaScaleFactor <<
        CSL_VPS_DEI_H_TNR_CFG2_TNR_LUMA_MV_SCALE_FACTOR_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG2_TNR_LUMA_MV_SCALE_FACTOR_MASK;
    regValue |= (tnrCfg->chromaScaleFoctor <<
        CSL_VPS_DEI_H_TNR_CFG2_TNR_CHROMA_MV_SCALE_FACTOR_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG2_TNR_CHROMA_MV_SCALE_FACTOR_MASK;
    regOvly->TNR_CFG2 = regValue;

    /* TNR_CFG3 */
    regValue = 0u;
    regValue |= (tnrCfg->lut[0u] <<
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT0_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT0_MASK;
    regValue |= (tnrCfg->lut[1u] <<
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT1_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT1_MASK;
    regValue |= (tnrCfg->lut[2u] <<
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT2_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT2_MASK;
    regValue |= (tnrCfg->lut[3u] <<
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT3_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT3_MASK;
    regOvly->TNR_CFG3 = regValue;

    /* TNR_CFG4 */
    regValue = 0u;
    regValue |= (tnrCfg->lut[4u] <<
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT4_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT4_MASK;
    regValue |= (tnrCfg->lut[5u] <<
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT5_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT5_MASK;
    regValue |= (tnrCfg->lut[6u] <<
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT6_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT6_MASK;
    regValue |= (tnrCfg->lut[7u] <<
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT7_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT7_MASK;
    regOvly->TNR_CFG4 = regValue;

    /* TNR_CFG5 */
    regValue = 0u;
    regValue |= (tnrCfg->lut[8u] <<
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT8_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT8_MASK;
    regValue |= (tnrCfg->lut[9u] <<
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT9_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT9_MASK;
    regValue |= (tnrCfg->lut[10u] <<
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT10_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT10_MASK;
    regValue |= (tnrCfg->lut[11u] <<
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT11_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT11_MASK;
    regOvly->TNR_CFG5 = regValue;

    /* TNR_CFG6 */
    regValue = 0u;
    regValue |= (tnrCfg->lut[12u] <<
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT12_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT12_MASK;
    regValue |= (tnrCfg->lut[13u] <<
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT13_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT13_MASK;
    regValue |= (tnrCfg->lut[14u] <<
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT14_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT14_MASK;
    regValue |= (tnrCfg->lut[15u] <<
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT15_SHIFT) &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT15_MASK;
    regOvly->TNR_CFG6 = regValue;

    return (VPS_SOK);
}



/* Functions to read advanced TNR configuration */
static Int32 vpsDeihGetAdvTnrCfg(VpsHal_Handle handle,
                                 Vps_DeiHqTnrConfig *tnrCfg)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != tnrCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* TNR_CFG1 */
    tnrCfg->lumaMvLowThr = (regOvly->TNR_CFG1 &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_SHIFT;
    tnrCfg->chromaMvLowThr = (regOvly->TNR_CFG1 &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_SHIFT;
    tnrCfg->lumaMvLowThrMax = (regOvly->TNR_CFG1 &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_MAX_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG1_TNR_LUMA_MV_LOW_THR_MAX_SHIFT;
    tnrCfg->chromaMvLowThrMax = (regOvly->TNR_CFG1 &
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_MAX_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG1_TNR_CHROMA_MV_LOW_THR_MAX_SHIFT;

    /* TNR_CFG2 */
    tnrCfg->lumaScaleFactor = (regOvly->TNR_CFG2 &
        CSL_VPS_DEI_H_TNR_CFG2_TNR_LUMA_MV_SCALE_FACTOR_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG2_TNR_LUMA_MV_SCALE_FACTOR_SHIFT;
    tnrCfg->chromaScaleFoctor = (regOvly->TNR_CFG2 &
        CSL_VPS_DEI_H_TNR_CFG2_TNR_CHROMA_MV_SCALE_FACTOR_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG2_TNR_CHROMA_MV_SCALE_FACTOR_SHIFT;

    /* TNR_CFG3 */
    tnrCfg->lut[0u] = (regOvly->TNR_CFG3 &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT0_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT0_SHIFT;
    tnrCfg->lut[1u] = (regOvly->TNR_CFG3 &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT1_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT1_SHIFT;
    tnrCfg->lut[2u] = (regOvly->TNR_CFG3 &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT2_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT2_SHIFT;
    tnrCfg->lut[3u] = (regOvly->TNR_CFG3 &
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT3_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG3_TNR_LUT3_SHIFT;

    /* TNR_CFG4 */
    tnrCfg->lut[4u] = (regOvly->TNR_CFG4 &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT4_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT4_SHIFT;
    tnrCfg->lut[5u] = (regOvly->TNR_CFG4 &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT5_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT5_SHIFT;
    tnrCfg->lut[6u] = (regOvly->TNR_CFG4 &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT6_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT6_SHIFT;
    tnrCfg->lut[7u] = (regOvly->TNR_CFG4 &
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT7_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG4_TNR_LUT7_SHIFT;

    /* TNR_CFG5 */
    tnrCfg->lut[8u] = (regOvly->TNR_CFG5 &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT8_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT8_SHIFT;
    tnrCfg->lut[9u] = (regOvly->TNR_CFG5 &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT9_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT9_SHIFT;
    tnrCfg->lut[10u] = (regOvly->TNR_CFG5 &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT10_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT10_SHIFT;
    tnrCfg->lut[11u] = (regOvly->TNR_CFG5 &
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT11_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG5_TNR_LUT11_SHIFT;

    /* TNR_CFG6 */
    tnrCfg->lut[12u] = (regOvly->TNR_CFG6 &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT12_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT12_SHIFT;
    tnrCfg->lut[13u] = (regOvly->TNR_CFG6 &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT13_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT13_SHIFT;
    tnrCfg->lut[14u] = (regOvly->TNR_CFG6 &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT14_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT14_SHIFT;
    tnrCfg->lut[15u] = (regOvly->TNR_CFG6 &
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT15_MASK) >>
        CSL_VPS_DEI_H_TNR_CFG6_TNR_LUT15_SHIFT;

    return (VPS_SOK);
}



/* Functions to write advanced FMD configuration */
static Int32 vpsDeihSetAdvFmdCfg(VpsHal_Handle handle,
                                 const Vps_DeiFmdConfig *fmdCfg)
{
    UInt32              regValue = 0u;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != fmdCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    regValue |= (fmdCfg->cafThr <<
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_THR_SHIFT) &
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_THR_MASK;
    regValue |= (fmdCfg->cafLineThr <<
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_LINE_THR_SHIFT) &
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_LINE_THR_MASK;
    regValue |= (fmdCfg->cafFieldThr <<
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_FIELD_THR_SHIFT) &
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_FIELD_THR_MASK;
    regOvly->FMD_CFG1 = regValue;

    return (VPS_SOK);
}



/* Functions to read advanced FMD configuration */
static Int32 vpsDeihGetAdvFmdCfg(VpsHal_Handle handle,
                                 Vps_DeiFmdConfig *fmdCfg)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != fmdCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    fmdCfg->cafThr = (regOvly->FMD_CFG1 &
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_THR_MASK) >>
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_THR_SHIFT;
    fmdCfg->cafLineThr = (regOvly->FMD_CFG1 &
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_LINE_THR_MASK) >>
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_LINE_THR_SHIFT;
    fmdCfg->cafFieldThr = (regOvly->FMD_CFG1 &
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_FIELD_THR_MASK) >>
        CSL_VPS_DEI_H_FMD_CFG1_FMD_CAF_FIELD_THR_SHIFT;

    fmdCfg->frameDiff = (regOvly->FMD_STAT0 &
        CSL_VPS_DEI_H_FMD_STAT0_FMD_FRAME_DIFF_MASK) >>
        CSL_VPS_DEI_H_FMD_STAT0_FMD_FRAME_DIFF_SHIFT;
    fmdCfg->fldDiff = (regOvly->FMD_STAT1 &
        CSL_VPS_DEI_H_FMD_STAT1_FMD_FIELD_DIFF_MASK) >>
        CSL_VPS_DEI_H_FMD_STAT1_FMD_FIELD_DIFF_SHIFT;
    fmdCfg->caf = (regOvly->FMD_STAT2 &
        CSL_VPS_DEI_H_FMD_STAT2_FMD_CAF_MASK) >>
        CSL_VPS_DEI_H_FMD_STAT2_FMD_CAF_SHIFT;
    fmdCfg->reset = (regOvly->FMD_STAT2 &
        CSL_VPS_DEI_H_FMD_STAT2_FMD_RESET_MASK) >>
        CSL_VPS_DEI_H_FMD_STAT2_FMD_RESET_SHIFT;
    return (VPS_SOK);
}



/* Functions to write advanced SNR configuration */
static Int32 vpsDeihSetAdvSnrCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqSnrConfig *snrCfg)
{
    UInt32              regValue;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != snrCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* SNR_CFG1 */
    regValue = 0u;
    regValue |= (snrCfg->sigmaThr[0u] <<
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_MASK;
    regValue |= (snrCfg->sigmaThr[1u] <<
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_MASK;
    regValue |= (snrCfg->sigmaThrMax[0u] <<
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_MAX_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_MAX_MASK;
    regValue |= (snrCfg->sigmaThrMax[1u] <<
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_MAX_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_MAX_MASK;
    regOvly->SNR_CFG1 = regValue;

    /* SNR_CFG2 */
    regValue = 0u;
    regValue |= (snrCfg->inrShift[0u] <<
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT1_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT1_MASK;
    regValue |= (snrCfg->inrShift[1u] <<
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT2_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT2_MASK;
    regValue |= (snrCfg->inrShift[2u] <<
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT3_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT3_MASK;
    regValue |= (snrCfg->inrTsmDeltaThr <<
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_TSM_DELTA_THR_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_TSM_DELTA_THR_MASK;
    regValue |= (snrCfg->intMedDeltaThr <<
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_MED_DELTA_THR_SHIFT) &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_MED_DELTA_THR_MASK;
    regOvly->SNR_CFG2 = regValue;

    return (VPS_SOK);
}



/* Functions to read advanced SNR configuration */
static Int32 vpsDeihGetAdvSnrCfg(VpsHal_Handle handle,
                                 Vps_DeiHqSnrConfig *snrCfg)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != snrCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* SNR_CFG1 */
    snrCfg->sigmaThr[0u] = (regOvly->SNR_CFG1 &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_SHIFT;
    snrCfg->sigmaThr[1u] = (regOvly->SNR_CFG1 &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_SHIFT;
    snrCfg->sigmaThrMax[0u] = (regOvly->SNR_CFG1 &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_MAX_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR1_MAX_SHIFT;
    snrCfg->sigmaThrMax[1u] = (regOvly->SNR_CFG1 &
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_MAX_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG1_SNR_SIGMA_THR2_MAX_SHIFT;

    /* SNR_CFG2 */
    snrCfg->inrShift[0u] = (regOvly->SNR_CFG2 &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT1_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT1_SHIFT;
    snrCfg->inrShift[1u] = (regOvly->SNR_CFG2 &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT2_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT2_SHIFT;
    snrCfg->inrShift[2u] = (regOvly->SNR_CFG2 &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT3_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_SHIFT3_SHIFT;
    snrCfg->inrTsmDeltaThr = (regOvly->SNR_CFG2 &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_TSM_DELTA_THR_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_TSM_DELTA_THR_SHIFT;
    snrCfg->intMedDeltaThr = (regOvly->SNR_CFG2 &
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_MED_DELTA_THR_MASK) >>
        CSL_VPS_DEI_H_SNR_CFG2_SNR_INR_MED_DELTA_THR_SHIFT;

    return (VPS_SOK);
}



/* Functions to write advanced GNM configuration */
static Int32 vpsDeihSetAdvGnmCfg(VpsHal_Handle handle,
                                 const Vps_DeiHqGnmConfig *gnmCfg)
{
    UInt32              regValue;
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != gnmCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* GNM_CFG0 */
    regValue = 0u;
    regValue |= (gnmCfg->lumaNoiseDev <<
        CSL_VPS_DEI_H_GNM_CFG0_GNM_Y_NOISE_DEV_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG0_GNM_Y_NOISE_DEV_MASK;
    regValue |= (gnmCfg->chromaNoiseDev <<
        CSL_VPS_DEI_H_GNM_CFG0_GNM_C_NOISE_DEV_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG0_GNM_C_NOISE_DEV_MASK;
    regOvly->GNM_CFG0 = regValue;

    /* GNM_CFG1 */
    regValue = 0u;
    regValue |= (gnmCfg->lumaDelta0 <<
        CSL_VPS_DEI_H_GNM_CFG1_GNM_Y_DELTA_0_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG1_GNM_Y_DELTA_0_MASK;
    regValue |= (gnmCfg->chromaDelta0 <<
        CSL_VPS_DEI_H_GNM_CFG1_GNM_C_DELTA_0_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG1_GNM_C_DELTA_0_MASK;
    regOvly->GNM_CFG1 = regValue;

    /* GNM_CFG2 */
    regValue = 0u;
    regValue |= (gnmCfg->lumaDelta1 <<
        CSL_VPS_DEI_H_GNM_CFG2_GNM_Y_DELTA_1_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG2_GNM_Y_DELTA_1_MASK;
    regValue |= (gnmCfg->chromaDelta1 <<
        CSL_VPS_DEI_H_GNM_CFG2_GNM_C_DELTA_1_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG2_GNM_C_DELTA_1_MASK;
    regOvly->GNM_CFG2 = regValue;

    /* GNM_CFG3 */
    regValue = 0u;
    regValue |= (gnmCfg->lumaDeltam <<
        CSL_VPS_DEI_H_GNM_CFG3_GNM_Y_DELTA_M_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG3_GNM_Y_DELTA_M_MASK;
    regValue |= (gnmCfg->chromaDeltam <<
        CSL_VPS_DEI_H_GNM_CFG3_GNM_C_DELTA_M_SHIFT) &
        CSL_VPS_DEI_H_GNM_CFG3_GNM_C_DELTA_M_MASK;
    regOvly->GNM_CFG3 = regValue;

    return (VPS_SOK);
}



/* Functions to read advanced GNM configuration */
static Int32 vpsDeihGetAdvGnmCfg(VpsHal_Handle handle,
                                 Vps_DeiHqGnmConfig *gnmCfg)
{
    CSL_VpsDeihRegsOvly regOvly;

    /* Check for the null */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != gnmCfg));

    regOvly = ((VpsHal_DeihObj *)handle)->baseAddress;

    /* GNM_CFG0 */
    gnmCfg->lumaNoiseDev = (regOvly->GNM_CFG0 &
        CSL_VPS_DEI_H_GNM_CFG0_GNM_Y_NOISE_DEV_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG0_GNM_Y_NOISE_DEV_SHIFT;
    gnmCfg->chromaNoiseDev = (regOvly->GNM_CFG0 &
        CSL_VPS_DEI_H_GNM_CFG0_GNM_C_NOISE_DEV_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG0_GNM_C_NOISE_DEV_SHIFT;

    /* GNM_CFG1 */
    gnmCfg->lumaDelta0 = (regOvly->GNM_CFG1 &
        CSL_VPS_DEI_H_GNM_CFG1_GNM_Y_DELTA_0_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG1_GNM_Y_DELTA_0_SHIFT;
    gnmCfg->chromaDelta0 = (regOvly->GNM_CFG1 &
        CSL_VPS_DEI_H_GNM_CFG1_GNM_C_DELTA_0_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG1_GNM_C_DELTA_0_SHIFT;

    /* GNM_CFG2 */
    gnmCfg->lumaDelta1 = (regOvly->GNM_CFG2 &
        CSL_VPS_DEI_H_GNM_CFG2_GNM_Y_DELTA_1_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG2_GNM_Y_DELTA_1_SHIFT;
    gnmCfg->chromaDelta1 = (regOvly->GNM_CFG2 &
        CSL_VPS_DEI_H_GNM_CFG2_GNM_C_DELTA_1_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG2_GNM_C_DELTA_1_SHIFT;

    /* GNM_CFG3 */
    gnmCfg->lumaDeltam = (regOvly->GNM_CFG3 &
        CSL_VPS_DEI_H_GNM_CFG3_GNM_Y_DELTA_M_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG3_GNM_Y_DELTA_M_SHIFT;
    gnmCfg->chromaDeltam = (regOvly->GNM_CFG3 &
        CSL_VPS_DEI_H_GNM_CFG3_GNM_C_DELTA_M_MASK) >>
        CSL_VPS_DEI_H_GNM_CFG3_GNM_C_DELTA_M_SHIFT;

    gnmCfg->yHistz = (regOvly->GNM_STAT0 &
        CSL_VPS_DEI_H_GNM_STAT0_GNM_Y_HIST_Z_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT0_GNM_Y_HIST_Z_SHIFT;
    gnmCfg->yHist0 = (regOvly->GNM_STAT1 &
        CSL_VPS_DEI_H_GNM_STAT1_GNM_Y_HIST_0_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT1_GNM_Y_HIST_0_SHIFT;
    gnmCfg->yHist1 = (regOvly->GNM_STAT2 &
        CSL_VPS_DEI_H_GNM_STAT2_GNM_Y_HIST_1_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT2_GNM_Y_HIST_1_SHIFT;
    gnmCfg->yHistm = (regOvly->GNM_STAT3 &
        CSL_VPS_DEI_H_GNM_STAT3_GNM_Y_HIST_M_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT3_GNM_Y_HIST_M_SHIFT;
    gnmCfg->cHistz = (regOvly->GNM_STAT4 &
        CSL_VPS_DEI_H_GNM_STAT4_GNM_C_HIST_Z_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT4_GNM_C_HIST_Z_SHIFT;
    gnmCfg->cHist0 = (regOvly->GNM_STAT5 &
        CSL_VPS_DEI_H_GNM_STAT5_GNM_C_HIST_0_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT5_GNM_C_HIST_0_SHIFT;
    gnmCfg->cHist1 = (regOvly->GNM_STAT6 &
        CSL_VPS_DEI_H_GNM_STAT6_GNM_C_HIST_1_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT6_GNM_C_HIST_1_SHIFT;
    gnmCfg->cHistm = (regOvly->GNM_STAT7 &
        CSL_VPS_DEI_H_GNM_STAT7_GNM_C_HIST_M_MASK) >>
        CSL_VPS_DEI_H_GNM_STAT7_GNM_C_HIST_M_SHIFT;
    return (VPS_SOK);
}
