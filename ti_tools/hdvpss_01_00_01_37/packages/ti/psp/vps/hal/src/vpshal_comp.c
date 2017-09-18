/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_comp.c
 *
 * \brief Compositor  HAL file.
 * This file implements the HAL APIs of the VPS Compositors.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/cslr/cslr_vps_comp.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_comp.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VPSHAL_COMP_ENABLE_PATH_MASK            (0x3Fu)
#define VPSHAL_COMP_INPUT_ENABLE_SHIFT          (0x05u)

/**
 *  enum VpsHal_CompConfigOvlyReg
 *  \brief Enums for the configuration overlay register offsets - used as array indices
 *  to register offset array in VpsHal_CompObj object. These enums defines the
 *  registers that will be used to form the configuration register overlay for
 *  DEI module. This in turn is used by client drivers to configure registers
 *  using VPDMA during context-switching (software channels with different
 *  configurations).
 *
 */
typedef enum
{
    VPSHAL_COMP_CONFIG_OVLY_COMP_HDMI_SETTINGS = 0,
    VPSHAL_COMP_CONFIG_OVLY_COMP_HDCOMP_SETTINGS,
    VPSHAL_COMP_CONFIG_OVLY_COMP_DVO2_SETTINGS,
    VPSHAL_COMP_CONFIG_OVLY_COMP_SD_SETTINGS,
    VPSHAL_COMP_CONFIG_OVLY_NUM_REG
    /**< This should be the last enum */
} VpsHal_CompConfigOvlyReg;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


typedef struct VpsHal_CompObj_t VpsHal_CompObj;

/**
 *  struct VpsHal_CompInfo
 *  \brief Structure containing per blender instance information for each COMP instance.
 *
 */
typedef struct
{
    VpsHal_CompId       compId;
    /**< Blender/Compositor id(like HDMI,SD,DVO2). */
    VpsHal_CompMode     mode;
    /**< stores mode - progressive or interlace */
    VpsHal_CompObj     *parent;
    /**< Points to overall compositor object */
    UInt32              isEnable;
    UInt32              configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay
         memory required DEI module */
    UInt32             *regOffset[1u];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA.*/
    UInt32             virRegOffset[1u];
    /**< Array of virtual register offsets in the VPDMA register
         configuration overlay memory corresponding to regOffset member.*/
} VpsHal_CompInfo;



/**
 * \brief struct VpsHal_CompInfo_t
 * Structure defining overall compositor object
 *
 */
struct VpsHal_CompObj_t
{
    UInt32                      instId;
    /**< COMP instance ID */
    CSL_Vps_compRegs            *regOvly;
    /**< Pointer to the COMP register overlay */
    VpsHal_CompErrorCallback     callbkfn;
    /**< Stores call back function which called during error condition */
    Ptr                          appPtr;
    /**< application pointer retuned along with callback. */
    VpsHal_CompInfo              compInfo[VPSHAL_COMP_NUM_MAX];
    /**< Pointer individual blender objects of compositor */
    UInt32                       isOpen;
    /**< Identifies whether specific compositor/bleber is open or not. */
    UInt32                       configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay
         memory required DEI module */
    UInt32                      *regOffset[VPSHAL_COMP_CONFIG_OVLY_NUM_REG];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA.*/
    UInt32                       virRegOffset[VPSHAL_COMP_CONFIG_OVLY_NUM_REG];
    /**< Array of virtual register offsets in the VPDMA register
         configuration overlay memory corresponding to regOffset member.*/
    UInt32                       statusReg;
    UInt32                       hdmiSettings;
    UInt32                       hdcompSettings;
    UInt32                       dvo2Settings;
    UInt32                       sdSettings;
};



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static UInt32 compConfigureHDMI(VpsHal_CompObj *compInfo,
                                const VpsHal_CompConfig *config);

static UInt32 compConfigureHDComp(VpsHal_CompObj *compInfo,
                                  const VpsHal_CompConfig *config);

static UInt32 compConfigureDvo2(VpsHal_CompObj *compInfo,
                                const VpsHal_CompConfig *config);

static UInt32 compConfigureSD(VpsHal_CompObj *compInfo,
                              const VpsHal_CompConfig *config);

static UInt32 compGetConfigHDMI(VpsHal_CompObj *compInfo,
                                VpsHal_CompConfig *config);

static UInt32 compGetConfigHDComp(VpsHal_CompObj *compInfo,
                                  VpsHal_CompConfig *config);

static UInt32 compGetConfigDvo2(VpsHal_CompObj *compInfo,
                                VpsHal_CompConfig *config);

static UInt32 compGetConfigSD(VpsHal_CompObj *compInfo,
                              VpsHal_CompConfig *config);


/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */

/**
 * \brief COMP objects - Module variable to store information about each COMP instance.
 * Note: If the number of COMP instance increase, then this variable should be
 * initialized with the added COMP instance information.
 */
static VpsHal_CompObj compObj[VPSHAL_COMP_MAX_INST] =
{
    VPSHAL_COMP_INST_0,
    NULL,
    NULL, NULL,
    {
        {
            VPSHAL_COMP_SELECT_HDMI,
            VPSHAL_COMP_MODE_PROGRESSIVE,
            NULL,
            FALSE,
            0u,
            {0u},
            {0u},
        },
        {
            VPSHAL_COMP_SELECT_HDCOMP,
            VPSHAL_COMP_MODE_PROGRESSIVE,
            NULL,
            FALSE,
            0u,
            {0u},
            {0u},
        },
        {
            VPSHAL_COMP_SELECT_DVO2,
            VPSHAL_COMP_MODE_PROGRESSIVE,
            NULL,
            FALSE,
            0u,
            {0u},
            {0u},
        },
        {
            VPSHAL_COMP_SELECT_SD,
            VPSHAL_COMP_MODE_PROGRESSIVE,
            NULL,
            FALSE,
            0u,
            {0u},
            {0u},
        }
    }, FALSE, 0u, {NULL}, {0u}, 0u, 0u, 0u, 0u, 0u
};



/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/**
 *  VpsHal_compInit
 *  \brief COMP HAL init function.
 *
 *  Initializes COMP objects, gets the register overlay offsets for COMP
 *  registers.
 *  This function should be called before calling any of COMP HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of COMP HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param numInstances NONE
 *  \param initParams   NONE
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_compInit(UInt32 numInstances,
                    const VpsHal_CompInstParams *initParams,
                    Ptr arg)
{
    Int                 instCnt;
    VpsHal_CompObj     *hcompObj;
    CSL_Vps_compRegs   *regOvly;
    Int                 ret = 0;
    VpsHal_CompInfo*    compInfo;
    VpsHal_CompId       blenderId;

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_COMP_MAX_INST));
    GT_assert(VpsHalTrace, (initParams != NULL));

    for (instCnt = 0; instCnt < numInstances; instCnt++)
    {
        GT_assert(VpsHalTrace,
            (initParams[instCnt].instId < VPSHAL_COMP_MAX_INST));

        hcompObj = &compObj[instCnt];
        hcompObj->instId = initParams[instCnt].instId;
        hcompObj->regOvly =
                (CSL_Vps_compRegs *)(initParams[instCnt].baseAddress);

        regOvly = hcompObj->regOvly;
        compInfo = hcompObj->compInfo;

        /* Following has been commented as now there is no need to have
         * separate overlays for each of the blenders. A Single overlay is
         * created and is used by the display controller driver. */
        for (blenderId = VPSHAL_COMP_SELECT_HDMI;
                blenderId < VPSHAL_COMP_NUM_MAX; blenderId ++)
        {
            compInfo[blenderId].compId = blenderId;
            compInfo[blenderId].parent = hcompObj;

            if (VPSHAL_COMP_SELECT_HDMI == blenderId)
            {
                compInfo[blenderId].regOffset[0u] =
                    (UInt32 *) &regOvly->HDMI_SETTINGS;
            }
            else if (VPSHAL_COMP_SELECT_HDCOMP == blenderId)
            {
                compInfo[blenderId].regOffset[0u] =
                    (UInt32 *) &regOvly->HDCOMP_SETTINGS;
            }
            else if (VPSHAL_COMP_SELECT_DVO2 == blenderId)
            {
                compInfo[blenderId].regOffset[0u] =
                    (UInt32 *) &regOvly->DVO2_SETTINGS;
            }
            else if (VPSHAL_COMP_SELECT_SD == blenderId)
            {
                compInfo[blenderId].regOffset[0u] =
                    (UInt32 *) &regOvly->SD_SETTINGS;
            }
            compInfo[blenderId].configOvlySize =
                VpsHal_vpdmaCalcRegOvlyMemSize(
                    compInfo[blenderId].regOffset,
                    1u,
                    compInfo[blenderId].virRegOffset);
        }

        hcompObj->regOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_HDMI_SETTINGS] =
                                        (UInt32 *) &regOvly->HDMI_SETTINGS;
        hcompObj->regOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_HDCOMP_SETTINGS] =
                                        (UInt32 *) &regOvly->HDCOMP_SETTINGS;
        hcompObj->regOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_DVO2_SETTINGS] =
                                        (UInt32 *) &regOvly->DVO2_SETTINGS;
        hcompObj->regOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_SD_SETTINGS] =
                                        (UInt32 *) &regOvly->SD_SETTINGS;

        /* Get the size of the overlay for COMP registers and the relative
         * virtual offset for the above registers when VPDMA config register
         * overlay is formed */
        hcompObj->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                      hcompObj->regOffset,
                                      VPSHAL_COMP_CONFIG_OVLY_NUM_REG,
                                      hcompObj->virRegOffset);

        /* Initialize all of the stored registers to zero */
        hcompObj->statusReg = 0u;
        hcompObj->hdmiSettings = 0u;
        hcompObj->hdcompSettings = 0u;
        hcompObj->dvo2Settings = 0u;
        hcompObj->sdSettings = 0u;

        /* Setting BackGround Color to White Initially */
        regOvly->BACK_COLOR_SETTINGS = VPS_CFG_COMP_BKCOLOR;
    }
    return (ret);
}



/**
 *  VpsHal_compDeInit
 *  \brief COMP HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_compDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_compOpen
 *  \brief Returns the handle to the requested COMP instance and
 *  compsoitor/blender(like HDMI, HDCOMP,DVO2,SD). It is allowed to open only one
 *  handle for each of compositors.
 *
 *  This function should be called prior to calling any of the COMP HAL
 *  configuration APIs.
 *
 *  VpsHal_init should be called prior to this.
 *
 *
 *  \param compInst  Requested COMP instance. Application handle which will be
 *                   returned along with callback.
 *  \param arg       NONE
 *
 *  \return          Returns COMP handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_compOpen(UInt32 compInst, Ptr arg)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle;

    handle = NULL;
    for (cnt = 0; cnt < VPSHAL_COMP_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (compInst == compObj[cnt].instId)
        {
            if(TRUE == compObj[cnt].isOpen)
            {
                /*Comp is alreday opened and so it is error*/
                handle = NULL;
                break;
            }

            /* Disable global interrupts */
            cookie = Hwi_disable();
            compObj[cnt].isOpen = TRUE;
            /* Enable global interrupts */
            Hwi_restore(cookie);

            /* Capture the handle to be retuned */
            handle = (VpsHal_Handle) &(compObj[cnt]);

            break;
        }
    }
    return (handle);
}




/**
 *  VpsHal_compClose
 *  \brief Closes the COMP HAL and compositor(like HDMI,SD etc) instance
 *
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle  Valid handle returned by VpsHal_compOpen function.
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_compClose(VpsHal_Handle handle)
{
    Int               ret     = -1;
    UInt32            cookie;
    VpsHal_CompObj   *compInfo;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    compInfo = (VpsHal_CompObj *)handle;

    if (FALSE == compInfo->isOpen)
    {
        /*this instance is alreday closed*/
        ret = -1;
    }
    else
    {
        /* Disable global interrupts */
        cookie = Hwi_disable();
        compInfo->isOpen = FALSE;
        /* Enable global interrupts */
        Hwi_restore(cookie);

        /* set all paramtesr to defalt or NULL*/
        compInfo->callbkfn = NULL;
        compInfo->appPtr = NULL;
        ret = 0;
    }

    return (ret);
}



/**
 *  VpsHal_compGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA
 *  register configuration overlay required to program the COMP registers using
 *  VPDMA config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register overlay
 *  used to program the COMP registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving valuable
 *  CPU in programming the VPS registers.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle Valid handle returned by VpsHal_compOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
UInt32 VpsHal_compGetConfigOvlySize(VpsHal_Handle handle, VpsHal_CompId compId)
{
    UInt32 configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Return the value already got in init function */
    configOvlySize = ((VpsHal_CompObj*) handle)->compInfo[compId].configOvlySize;

    return (configOvlySize);
}



 /**
 *  VpsHal_compCreateConfigOvly
 *  \brief Creates the COMP register configuration
 *  overlay using VPDMA helper function.
 *
 *  This function does not initialize the overlay with COMP configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  Comp_setConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  \param handle         Valid handle returned by VpsHal_compOpen function.
 *
 *  \param configOvlyPtr  Pointer to the overlay memory where the overlay is
 *                        formed. The memory for the overlay should be allocated
 *                        by client driver. This parameter should be non-NULL.
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_compCreateConfigOvly(VpsHal_Handle handle,
                                VpsHal_CompId compId,
                                Ptr configOvlyPtr)
{
    Int ret;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    ret = VpsHal_vpdmaCreateRegOverlay(
              ((VpsHal_CompObj*) handle)->compInfo[compId].regOffset,
              1u,
              configOvlyPtr);

    return (ret);
}


/**
 *  Comp_setConfig
 *  \brief Sets the entire COMP configuration to either the
 *  actual COMP registers or to the configuration overlay memory.
 *
 *  This function configures the entire COMP registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual COMP MMR or
 *  to configuration overlay memory.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  COMP registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle        Valid handle returned by VpsHal_compOpen function.
 *
 *  \param config        Pointer to VpsHal_CompConfig structure containing the
 *                       register configurations. This parameter should be
 *                       non-NULL.
 *  \param compId        NONE
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual COMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_compSetConfig(VpsHal_Handle handle,
                         const VpsHal_CompConfig *config,
                         VpsHal_CompId compId,
                         Ptr configOvlyPtr)
{
    VpsHal_CompObj    *compInfo = NULL;
    CSL_Vps_compRegs  *regOvly = NULL;
    UInt32             tempReg = 0;
    UInt32            *virRegOffset = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    compInfo = (VpsHal_CompObj *)handle;
    regOvly = compInfo->regOvly;

    switch (compId)
    {
        case VPSHAL_COMP_SELECT_HDMI :
            tempReg = compConfigureHDMI(compInfo, config);
            if (NULL == configOvlyPtr)
            {
                /* Write to the actual register */
                regOvly->HDMI_SETTINGS = tempReg;
            }
            else
            {
               /* Write to config register overlay at the correct offset. Since the
                * offset is in words, it can be directly added to the pointer so that
                * (UInt32 *) addition will result in proper offset value */
                virRegOffset = (UInt32 *) configOvlyPtr +
                        compInfo->virRegOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_HDMI_SETTINGS];
                *virRegOffset = tempReg;
            }
            /* Store the temp register in the local copy of the register
             * as well. */
            compInfo->hdmiSettings = tempReg;
        break;

        case VPSHAL_COMP_SELECT_HDCOMP :
            tempReg = compConfigureHDComp(compInfo, config);
            if (NULL == configOvlyPtr)
            {
                /* Write to the actual register */
                regOvly->HDCOMP_SETTINGS = tempReg;
            }
            else
            {
                /* Write to config register overlay at the correct offset. Since the
                 * offset is in words, it can be directly added to the pointer so that
                 * (UInt32 *) addition will result in proper offset value */
                virRegOffset = (UInt32 *) configOvlyPtr +
                        compInfo->virRegOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_HDCOMP_SETTINGS];
                *virRegOffset = tempReg;
            }
            /* Store the temp register in the local copy of the register
             * as well. */
            compInfo->hdcompSettings = tempReg;
        break;

        case VPSHAL_COMP_SELECT_DVO2 :
            tempReg = compConfigureDvo2(compInfo, config);
            if (NULL == configOvlyPtr)
            {
                /* Write to the actual register */
                regOvly->DVO2_SETTINGS = tempReg;
            }
            else
            {
                /* Write to config register overlay at the correct offset. Since the
                * offset is in words, it can be directly added to the pointer so that
                * (UInt32 *) addition will result in proper offset value */
                virRegOffset = (UInt32 *) configOvlyPtr +
                        compInfo->virRegOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_DVO2_SETTINGS];
                *virRegOffset = tempReg;
            }
            /* Store the temp register in the local copy of the register
             * as well. */
            compInfo->dvo2Settings = tempReg;
        break;

        case VPSHAL_COMP_SELECT_SD :
            tempReg = compConfigureSD(compInfo, config);
            if (NULL == configOvlyPtr)
            {
                /* Write to the actual register */
                regOvly->SD_SETTINGS = tempReg;
            }
            else
            {
                /* Write to config register overlay at the correct offset. Since the
                * offset is in words, it can be directly added to the pointer so that
                * (UInt32 *) addition will result in proper offset value */
                virRegOffset = (UInt32 *) configOvlyPtr +
                        compInfo->virRegOffset[VPSHAL_COMP_CONFIG_OVLY_COMP_SD_SETTINGS];
                *virRegOffset = tempReg;
            }
            /* Store the temp register in the local copy of the register
             * as well. */
            compInfo->sdSettings = tempReg;
        break;

        default:
        break;
    }

    return (0);
}



/**
 *  VpsHal_compGetConfig
 *  \brief Gets the entire COMP configuration from the actual
 *  COMP registers.
 *
 *  This function gets the entire COMP configuration.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle Valid handle returned by VpsHal_compOpen function.
 *
 *  \param config Pointer to VpsHal_CompConfig structure to be filled with
 *                register configurations. This parameter should be non-NULL.
 *  \param compId None
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_compGetConfig(VpsHal_Handle handle,
                         VpsHal_CompConfig *config,
                         VpsHal_CompId compId)
{
    VpsHal_CompObj  *compInfo = NULL;
    UInt32           retCode = 0;
    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    compInfo = (VpsHal_CompObj *)handle;

    switch (compId)
    {
        case VPSHAL_COMP_SELECT_HDMI :
            /* Get HDMI Blender configuration */
            retCode = compGetConfigHDMI(compInfo, config);
        break;

        case VPSHAL_COMP_SELECT_HDCOMP :
            /* Get HD COMP Blender configuration */
            retCode = compGetConfigHDComp(compInfo, config);
        break;

        case VPSHAL_COMP_SELECT_DVO2 :
        /* Get HD 2 Blender configuration */
            retCode = compGetConfigDvo2(compInfo, config);
        break;

        case VPSHAL_COMP_SELECT_SD :
        /* Get SD Blender configuration */
            retCode = compGetConfigSD(compInfo, config);
        break;

        default:
        break;
    }

    return (retCode);
}



/**
 *  VpsHal_compSetBackGrColor
 *  \brief Sets background color.
 *
 *  This backround color is common for all the compositors/blenders(like HDMI,
 *  HDCOMP,DVO2, SD). It means it will affect all compositors/blenders if it is
 *  changed for any of compositors/blenders.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  backround color is set.
 *
 *  \param handle       Valid handle returned by VpsHal_compOpen function.
 *  \param backgrdColor 30 bit backround color in RGB format.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_compSetBackGrColor(VpsHal_Handle handle,
                              UInt32 backgrdColor)
{
    VpsHal_CompObj            *compInfo = NULL;
    CSL_Vps_compRegs    *regOvly = NULL;

    GT_assert(VpsHalTrace, (NULL != handle));

    compInfo = (VpsHal_CompObj*)handle;
    regOvly = compInfo->regOvly;

    /* Backround color common for all compositors/Blender */
    /* Write to the actual register */
    regOvly->BACK_COLOR_SETTINGS =
               (CSL_VPS_COMP_BACK_COLOR_SETTINGS_BACK_CLR_MASK & backgrdColor);

    return (0);
}



/**
 *  VpsHal_compGetBackGrColor
 *  \brief Gets background color.
 *
 *  This backround color is common for all the compositors/blenders(like HDMI,
 *  HDCOMP,DVO2, SD).
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle       Valid handle returned by VpsHal_compOpen function.
 *  \param backgrdColor 30 bit backround color in RGB format.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_compGetBackGrColor(VpsHal_Handle handle,
                              UInt32* backgrdColor)
{
    VpsHal_CompObj            *compInfo = NULL;
    CSL_Vps_compRegs    *regOvly = NULL;

    GT_assert(VpsHalTrace, (NULL != handle));

    compInfo = (VpsHal_CompObj*)handle;
    regOvly = compInfo->regOvly;

      /* Backround color common for all compositors/Blender */
    *backgrdColor = (regOvly->BACK_COLOR_SETTINGS &
                  CSL_VPS_COMP_BACK_COLOR_SETTINGS_BACK_CLR_MASK);

    return (0);
}



/**
 *  VpsHal_CompGetErrorSatus
 *  \brief Gets error bits for respective
 *  compositors/blenders. Please note that Compositor does not handle ISR.
 *  This API has been provided API like VpsHal_CompGetErrorSatus for query the error
 *  for any of compositors out of HDMI, HDCOMP, SD and DVO2. Once this API is
 *  called for any of compsoitors/blenders, compositor/blenders will call this
 *  callbacks for other comp provided error bit is set.
 *  For example, HDMI calls VpsHal_CompGetErrorSatus and so error value will be
 *  returned for HDMI and call back will be called for other compositors
 *  provided error has occured.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  gets error condition.
 *
 *  \param handle    Valid handle returned by VpsHal_compOpen function.
 *  \param errStatus Error bits will returned. This errStatus code could
 *                   have several errors and one bit is set for each error.
 *                   Mask for these bits are difined and it is same as STATU
 *                   regiter of compositor.
 *
 *  \return          Returns 0 on success else returns error value.
 */
Int VpsHal_CompGetErrorSatus(VpsHal_Handle handle, UInt32 *errStatus)
{
    VpsHal_CompObj      *hcompObj = NULL;
    CSL_Vps_compRegs    *regOvly = NULL;
    /*UInt32               tempReg = 0;
    UInt32               errorCode = 0;
    UInt32               errMask = 0;*/

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != errStatus));


    hcompObj = (VpsHal_CompObj*)handle;
    regOvly = hcompObj->regOvly;
    *errStatus = regOvly->STATUS;
    return (0);
}



/**
 *  VpsHal_compEnableOutput
 *  \brief Enable respective compositor/blender.
 *
 *  VpsHal_init, VpsHal_compOpen and Comp_setConfig should be called prior
 *  to this.
 *
 *
 *  \param handle   Valid handle returned by VpsHal_compOpen function.
 *  \param compId   None
 *  \param mode     Interlace or progressive
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_compEnableOutput(VpsHal_Handle handle,
                            VpsHal_CompId compId,
                            VpsHal_CompMode mode)
{
    VpsHal_CompObj      *hcompObj = NULL;
    CSL_Vps_compRegs    *regOvly = NULL;
    UInt32               tempReg = 0;


    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    hcompObj = (VpsHal_CompObj*)handle;
    regOvly = hcompObj->regOvly;

    /* Enable and mode bit has to be set in status regiter.
     * Reading STATUS rtegiter will cause clear of status bits and so data has
     * been created in temp regiter before writing to compositor regiter */
    tempReg = hcompObj->statusReg;

    /* Create value for requested compositor to be enabled */
    switch (compId)
    {
        case VPSHAL_COMP_SELECT_HDMI :
            /* Enable HDMI compositor/blender and set mode */
            tempReg &= (~CSL_VPS_COMP_STATUS_HDMI_FMT_MASK);
            if(VPSHAL_COMP_MODE_INTERLACED == mode)
            {
                tempReg |= CSL_VPS_COMP_STATUS_HDMI_FMT_MASK;
            }
            hcompObj->compInfo[VPSHAL_COMP_SELECT_HDMI].mode = mode;

            tempReg |=  CSL_VPS_COMP_STATUS_HDMI_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_HDMI].isEnable = TRUE;
        break;

        case VPSHAL_COMP_SELECT_HDCOMP :
            /* Enable HDCOMP compositor/blender and set mode */
            tempReg &= (~CSL_VPS_COMP_STATUS_HDCOMP_FMT_MASK);
            if(VPSHAL_COMP_MODE_INTERLACED == mode)
            {
                tempReg |= CSL_VPS_COMP_STATUS_HDCOMP_FMT_MASK;
            }
            hcompObj->compInfo[VPSHAL_COMP_SELECT_HDCOMP].mode = mode;

            tempReg |=  CSL_VPS_COMP_STATUS_HDCOMP_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_HDCOMP].isEnable = TRUE;
        break;

        case VPSHAL_COMP_SELECT_DVO2 :
            /* Enable DVO2 compositor/blender and set mode */
            tempReg &= (~CSL_VPS_COMP_STATUS_DVO2_FMT_MASK);
            if(VPSHAL_COMP_MODE_INTERLACED == mode)
            {
                tempReg |=  CSL_VPS_COMP_STATUS_DVO2_FMT_MASK;
            }
            hcompObj->compInfo[VPSHAL_COMP_SELECT_DVO2].mode = mode;

            tempReg |= CSL_VPS_COMP_STATUS_DVO2_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_DVO2].isEnable = TRUE;
        break;

        case VPSHAL_COMP_SELECT_SD :
            /* Enable SD compositor/blender and set mode */
            tempReg &= (~CSL_VPS_COMP_STATUS_SD_FMT_MASK);
            if(VPSHAL_COMP_MODE_INTERLACED == mode)
            {
                tempReg |= CSL_VPS_COMP_STATUS_SD_FMT_MASK;
            }
            hcompObj->compInfo[VPSHAL_COMP_SELECT_SD].mode = mode;

            tempReg |= CSL_VPS_COMP_STATUS_SD_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_SD].isEnable = TRUE;
        break;

        default:
        break;
    }

    /* Enable the compositor/blender*/
    hcompObj->statusReg = tempReg;
    regOvly->STATUS = hcompObj->statusReg;

    return (0);
}



/**
 *  VpsHal_compDisableOuput
 *  \brief Enable respective compositor/blender.
 *
 *
 *  VpsHal_init, VpsHal_compOpen, Comp_setConfig and Comp_disableOuput
 *  should be called prior to this.
 *
 *
 *  \param handle  Valid handle returned by VpsHal_compOpen function.
 *  \param compId  None
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_compDisableOutput(VpsHal_Handle handle, VpsHal_CompId compId)
{
    CSL_Vps_compRegs    *regOvly = NULL;
    VpsHal_CompObj            *hcompObj = NULL;
    UInt32               tempReg=0;


    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    hcompObj = (VpsHal_CompObj *)handle;
    regOvly = hcompObj->regOvly;

    /* Enable and mode bit has to be set in status regiter.
     * Reading STATUS rtegiter will cause clear of status bits and so data has
     * been created in temp regiter before writing to compositor regiter */
    tempReg = hcompObj->statusReg;

    switch (compId)
    {
        case VPSHAL_COMP_SELECT_HDMI :
            /* Disbale HDMI compositor/blender. */
            tempReg &= ~CSL_VPS_COMP_STATUS_HDMI_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_HDMI].isEnable = FALSE;
        break;

        case VPSHAL_COMP_SELECT_HDCOMP :
            /* Disbale HD COMP compositor/blender. */
            tempReg &= ~CSL_VPS_COMP_STATUS_HDCOMP_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_HDCOMP].isEnable = FALSE;
        break;

        case VPSHAL_COMP_SELECT_DVO2 :
            /* Disbale HD 2 compositor/blender. */
            regOvly->STATUS &= ~CSL_VPS_COMP_STATUS_DVO2_ENABLE_MASK;
            hcompObj->compInfo[VPSHAL_COMP_SELECT_DVO2].isEnable = FALSE;
        break;

        case VPSHAL_COMP_SELECT_SD :
             /* Disbale SD compositor/blender. */
             tempReg &= ~CSL_VPS_COMP_STATUS_SD_ENABLE_MASK;
             hcompObj->compInfo[VPSHAL_COMP_SELECT_SD].isEnable = FALSE;
        break;

        default:
        break;
    }

    hcompObj->statusReg = tempReg;
    regOvly->STATUS = hcompObj->statusReg;
    return (0);
}



Int32 VpsHal_compEnableInput(VpsHal_Handle handle,
                             VpsHal_CompId compId,
                             UInt32 inputNum,
                             Ptr configOvlyPtr,
                             UInt32 isEnable)
{
    Int32                retVal = 0;
    VpsHal_CompObj      *hcompObj = NULL;
    CSL_Vps_compRegs    *regOvly = NULL;
    UInt32              *tempReg = NULL, regValue;
    UInt32              *ovlyPtr = NULL;
    volatile UInt32     *regPtr = NULL;
    UInt32               virtRegOffset;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    hcompObj = (VpsHal_CompObj*)handle;
    regOvly = hcompObj->regOvly;

    if (VPSHAL_COMP_SELECT_SD == compId)
    {
        if (inputNum >= 1u)
        {
            inputNum += 2u;
        }
    }
    else if (VPSHAL_COMP_SELECT_HDCOMP == compId)
    {
        inputNum ++;
    }
    else
    {
        if (inputNum >= 1u)
        {
            inputNum += 1u;
        }
    }
    switch (compId)
    {
        /* Get the pointer to the stored register, offset of the register
         * in the overlay and pointer to the actual register as per the
         * given CompId. Since settings register for the compositor have
         * same format, pointer to actual register is taken and then used
         * for enabling specific output. */
        case VPSHAL_COMP_SELECT_HDMI :
            tempReg = &(hcompObj->hdmiSettings);
            regPtr = &(regOvly->HDMI_SETTINGS);
            virtRegOffset =
                hcompObj->compInfo[compId].virRegOffset[0u];
            break;

        case VPSHAL_COMP_SELECT_HDCOMP :
            tempReg = &(hcompObj->hdcompSettings);
            regPtr = &(regOvly->HDCOMP_SETTINGS);
            virtRegOffset =
                hcompObj->compInfo[compId].virRegOffset[0u];
            break;

        case VPSHAL_COMP_SELECT_DVO2 :
            tempReg = &(hcompObj->dvo2Settings);
            regPtr = &(regOvly->DVO2_SETTINGS);
            virtRegOffset =
                hcompObj->compInfo[compId].virRegOffset[0u];
            break;

        case VPSHAL_COMP_SELECT_SD :
            tempReg = &(hcompObj->sdSettings);
            regPtr = &(regOvly->SD_SETTINGS);
            virtRegOffset =
                hcompObj->compInfo[compId].virRegOffset[0u];
            break;

        default:
            regPtr = NULL;
            retVal = -1;
            break;
    }

    if (0 == retVal)
    {
        regValue = *tempReg;
        inputNum = (VPSHAL_COMP_INPUT_ENABLE_SHIFT - inputNum);
        if (TRUE == isEnable)
        {
            regValue |= (0x1u << inputNum);
        }
        else
        {
            regValue &= (~(0x1u << inputNum));
        }
        *tempReg = regValue;
        if (NULL != configOvlyPtr)
        {
            /* enable the input for the given blender in the register overlay */
            ovlyPtr = (UInt32 *) configOvlyPtr + virtRegOffset;
            *ovlyPtr = regValue;
        }
        else
        {
            /* enable the input for the given blender in the register */
            *regPtr = regValue;
        }
    }

    return (retVal);
}



static UInt32 compGetConfigHDMI(VpsHal_CompObj *compInfo,
                                VpsHal_CompConfig *config)
{
    UInt32              tempReg = 0;
    UInt32              dispCnt = 0;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    //regOvly = compInfo->regOvly;
    tempReg = compInfo->hdmiSettings;

    /* Data from video alpha or final alpha blending */
    config->fbPath =
    (VpsHal_CompFeedbkPathSelect)((tempReg &
                             CSL_VPS_COMP_HDMI_SETTINGS_FB_SEL_MASK) >>
                             CSL_VPS_COMP_HDMI_SETTINGS_FB_SEL_SHIFT);

    /* check Global blening re-order enable or disbale */
    config->gReorderMode=
    (VpsHal_CompGlobalReorderMode)((tempReg &
                             CSL_VPS_COMP_HDMI_SETTINGS_G_ORDER_MASK) >>
                             CSL_VPS_COMP_HDMI_SETTINGS_G_ORDER_SHIFT);

    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            /* Get the blending order between G1, G2, G3 and VID */
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    config->displayOrder[dispCnt] =
                                   (VpsHal_CompDisplayOrder)((tempReg &
                                   CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_MASK)>>
                                   CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_HDMI_SETTINGS_G0_ORDER_MASK)>>
                                    CSL_VPS_COMP_HDMI_SETTINGS_G0_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_HDMI_SETTINGS_G1_ORDER_MASK)>>
                                    CSL_VPS_COMP_HDMI_SETTINGS_G1_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_HDMI_SETTINGS_G2_ORDER_MASK)>>
                                    CSL_VPS_COMP_HDMI_SETTINGS_G2_ORDER_SHIFT);
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Get priority for Video layer in the case g_reorder is
        * set to 0 */
        config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] =
                                (VpsHal_CompDisplayOrder)((tempReg &
                                CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_MASK)>>
                                CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_SHIFT);
    }

    /* Get different i/p enabled or disbled */
    config->grpx0InEnable =
                      ((tempReg & CSL_VPS_COMP_HDMI_SETTINGS_GRPX0_EN_MASK) >>
                      CSL_VPS_COMP_HDMI_SETTINGS_GRPX0_EN_SHIFT);

    config->grpx1InEnable =
                      ((tempReg & CSL_VPS_COMP_HDMI_SETTINGS_GRPX1_EN_MASK) >>
                      CSL_VPS_COMP_HDMI_SETTINGS_GRPX1_EN_SHIFT);

    config->grpx2InEnable =
                      ((tempReg & CSL_VPS_COMP_HDMI_SETTINGS_GRPX2_EN_MASK) >>
                      CSL_VPS_COMP_HDMI_SETTINGS_GRPX2_EN_SHIFT);

    config->videoInConfig.hdmiConfig.enableHdVid =
                      ((tempReg & CSL_VPS_COMP_HDMI_SETTINGS_VID_EN_MASK) >>
                      CSL_VPS_COMP_HDMI_SETTINGS_VID_EN_SHIFT);

    config->videoInConfig.hdmiConfig.enableHdPip =
                      ((tempReg & CSL_VPS_COMP_HDMI_SETTINGS_PIP_EN_MASK) >>
                      CSL_VPS_COMP_HDMI_SETTINGS_PIP_EN_SHIFT);

    config->videoInConfig.hdmiConfig.hdmiBase =
                    (VpsHal_CompHdmiBlendBase)((tempReg &
                              CSL_VPS_COMP_HDMI_SETTINGS_VID_BLD_ORDER_MASK) >>
                              CSL_VPS_COMP_HDMI_SETTINGS_VID_BLD_ORDER_SHIFT);

    return (0);
}



static UInt32 compGetConfigHDComp(VpsHal_CompObj *compInfo,
                                  VpsHal_CompConfig *config)
{
    UInt32              tempReg = 0u;
    UInt32              dispCnt = 0u;
    //CSL_Vps_compRegs   *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    //regOvly = compInfo->regOvly;
    tempReg = compInfo->hdcompSettings;
    //tempReg = regOvly->HDCOMP_SETTINGS;

    config->fbPath =
    (VpsHal_CompFeedbkPathSelect)((tempReg &
                             CSL_VPS_COMP_HDCOMP_SETTINGS_FB_SEL_MASK) >>
                             CSL_VPS_COMP_HDCOMP_SETTINGS_FB_SEL_SHIFT);

    config->gReorderMode=
    (VpsHal_CompGlobalReorderMode)((tempReg &
                             CSL_VPS_COMP_HDCOMP_SETTINGS_G_ORDER_MASK) >>
                             CSL_VPS_COMP_HDCOMP_SETTINGS_G_ORDER_SHIFT);

    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            /* Get different blending /display re-order */
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    config->displayOrder[dispCnt] =
                                (VpsHal_CompDisplayOrder)((tempReg &
                                CSL_VPS_COMP_HDCOMP_SETTINGS_VID_ORDER_MASK)>>
                                CSL_VPS_COMP_HDCOMP_SETTINGS_VID_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    config->displayOrder[dispCnt] =
                                 (VpsHal_CompDisplayOrder)((tempReg &
                                 CSL_VPS_COMP_HDCOMP_SETTINGS_G0_ORDER_MASK)>>
                                 CSL_VPS_COMP_HDCOMP_SETTINGS_G0_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    config->displayOrder[dispCnt] =
                                 (VpsHal_CompDisplayOrder)((tempReg &
                                 CSL_VPS_COMP_HDCOMP_SETTINGS_G1_ORDER_MASK)>>
                                 CSL_VPS_COMP_HDCOMP_SETTINGS_G1_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    config->displayOrder[dispCnt] =
                                 (VpsHal_CompDisplayOrder)((tempReg &
                                 CSL_VPS_COMP_HDCOMP_SETTINGS_G2_ORDER_MASK)>>
                                 CSL_VPS_COMP_HDCOMP_SETTINGS_G2_ORDER_SHIFT);
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Get priority for Video layer in the case g_reorder is
        * set to 0 */
        config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] =
                                (VpsHal_CompDisplayOrder)((tempReg &
                                CSL_VPS_COMP_HDCOMP_SETTINGS_VID_ORDER_MASK)>>
                                CSL_VPS_COMP_HDCOMP_SETTINGS_VID_ORDER_SHIFT);
    }

    /* Get different i/p enabled or disbled */
    config->grpx0InEnable =
                    ((tempReg & CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX0_EN_MASK) >>
                    CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX0_EN_SHIFT);

    config->grpx1InEnable =
                    ((tempReg & CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX1_EN_MASK) >>
                    CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX1_EN_SHIFT);

    config->grpx2InEnable =
                    ((tempReg & CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX2_EN_MASK) >>
                    CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX2_EN_SHIFT);

    config->videoInConfig.hdCompConfig.enableHdCit =
                    ((tempReg & CSL_VPS_COMP_HDCOMP_SETTINGS_VID_EN_MASK) >>
                    CSL_VPS_COMP_HDCOMP_SETTINGS_VID_EN_SHIFT);

    config->videoInConfig.hdCompConfig.enableHdPip =
                    ((tempReg & CSL_VPS_COMP_HDCOMP_SETTINGS_PIP_EN_MASK) >>
                    CSL_VPS_COMP_HDCOMP_SETTINGS_PIP_EN_SHIFT);

    config->videoInConfig.hdCompConfig.hdCompBase =
                    (VpsHal_CompHdCompBlendBase)((tempReg &
                           CSL_VPS_COMP_HDCOMP_SETTINGS_VID_BLD_ORDER_MASK) >>
                           CSL_VPS_COMP_HDCOMP_SETTINGS_VID_BLD_ORDER_SHIFT);

    return (0);
}



static UInt32 compGetConfigDvo2(VpsHal_CompObj *compInfo,
                                VpsHal_CompConfig *config)
{
    UInt32              tempReg = 0u;
    UInt32              dispCnt = 0u;
    //CSL_Vps_compRegs   *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    //regOvly = compInfo->regOvly;
    tempReg = compInfo->dvo2Settings;
    //tempReg = regOvly->DVO2_SETTINGS;

    config->fbPath =
    (VpsHal_CompFeedbkPathSelect)((tempReg &
                             CSL_VPS_COMP_DVO2_SETTINGS_FB_SEL_MASK) >>
                             CSL_VPS_COMP_DVO2_SETTINGS_FB_SEL_SHIFT);

    config->gReorderMode=
    (VpsHal_CompGlobalReorderMode)((tempReg &
                             CSL_VPS_COMP_DVO2_SETTINGS_G_ORDER_MASK) >>
                             CSL_VPS_COMP_DVO2_SETTINGS_G_ORDER_SHIFT);

    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            /* Get different blending /display re-order */
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    config->displayOrder[dispCnt] =
                                   (VpsHal_CompDisplayOrder)((tempReg &
                                   CSL_VPS_COMP_DVO2_SETTINGS_VID_ORDER_MASK)>>
                                   CSL_VPS_COMP_DVO2_SETTINGS_VID_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_DVO2_SETTINGS_G0_ORDER_MASK)>>
                                    CSL_VPS_COMP_DVO2_SETTINGS_G0_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_DVO2_SETTINGS_G1_ORDER_MASK)>>
                                    CSL_VPS_COMP_DVO2_SETTINGS_G1_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_DVO2_SETTINGS_G2_ORDER_MASK)>>
                                    CSL_VPS_COMP_DVO2_SETTINGS_G2_ORDER_SHIFT);
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Get priority for Video layer in the case g_reorder is
        * set to 0 */
        config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] =
                                   (VpsHal_CompDisplayOrder)((tempReg &
                                   CSL_VPS_COMP_DVO2_SETTINGS_VID_ORDER_MASK)>>
                                   CSL_VPS_COMP_DVO2_SETTINGS_VID_ORDER_SHIFT);
    }

    /* Get different i/p enabled or disbled */
    config->grpx0InEnable =
                      ((tempReg & CSL_VPS_COMP_DVO2_SETTINGS_GRPX0_EN_MASK) >>
                      CSL_VPS_COMP_DVO2_SETTINGS_GRPX0_EN_SHIFT);

    config->grpx1InEnable =
                      ((tempReg & CSL_VPS_COMP_DVO2_SETTINGS_GRPX1_EN_MASK) >>
                      CSL_VPS_COMP_DVO2_SETTINGS_GRPX1_EN_SHIFT);

    config->grpx2InEnable =
                      ((tempReg & CSL_VPS_COMP_DVO2_SETTINGS_GRPX2_EN_MASK) >>
                      CSL_VPS_COMP_DVO2_SETTINGS_GRPX2_EN_SHIFT);

    config->videoInConfig.dvo2Config.enableHdVid =
                      ((tempReg & CSL_VPS_COMP_DVO2_SETTINGS_VID_EN_MASK) >>
                      CSL_VPS_COMP_DVO2_SETTINGS_VID_EN_SHIFT);

    config->videoInConfig.dvo2Config.enableHdPip =
                      ((tempReg & CSL_VPS_COMP_DVO2_SETTINGS_PIP_EN_MASK) >>
                      CSL_VPS_COMP_DVO2_SETTINGS_PIP_EN_SHIFT);

    config->videoInConfig.dvo2Config.dvo2Base =
                    (VpsHal_CompDvo2BlendBase)((tempReg &
                              CSL_VPS_COMP_DVO2_SETTINGS_VID_BLD_ORDER_MASK) >>
                              CSL_VPS_COMP_DVO2_SETTINGS_VID_BLD_ORDER_SHIFT);

    return (0);
}



static UInt32 compGetConfigSD(VpsHal_CompObj *compInfo,
                              VpsHal_CompConfig *config)
{
    UInt32             tempReg = 0u;
    UInt32             dispCnt = 0u;
    //CSL_Vps_compRegs  *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    //regOvly = compInfo->regOvly;
    tempReg = compInfo->sdSettings;
    //tempReg = regOvly->SD_SETTINGS;

    config->fbPath =
    (VpsHal_CompFeedbkPathSelect)((tempReg &
                             CSL_VPS_COMP_SD_SETTINGS_FB_SEL_MASK) >>
                             CSL_VPS_COMP_SD_SETTINGS_FB_SEL_SHIFT);

    config->gReorderMode=
    (VpsHal_CompGlobalReorderMode)((tempReg &
                             CSL_VPS_COMP_SD_SETTINGS_G_ORDER_MASK) >>
                             CSL_VPS_COMP_SD_SETTINGS_G_ORDER_SHIFT);

    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    config->displayOrder[dispCnt] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_SD_SETTINGS_VID_ORDER_MASK)>>
                                    CSL_VPS_COMP_SD_SETTINGS_VID_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    config->displayOrder[dispCnt] =
                                     (VpsHal_CompDisplayOrder)((tempReg &
                                     CSL_VPS_COMP_SD_SETTINGS_G0_ORDER_MASK)>>
                                     CSL_VPS_COMP_SD_SETTINGS_G0_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    config->displayOrder[dispCnt] =
                                     (VpsHal_CompDisplayOrder)((tempReg &
                                     CSL_VPS_COMP_SD_SETTINGS_G1_ORDER_MASK)>>
                                     CSL_VPS_COMP_SD_SETTINGS_G1_ORDER_SHIFT);
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    config->displayOrder[dispCnt] =
                                     (VpsHal_CompDisplayOrder)((tempReg &
                                     CSL_VPS_COMP_SD_SETTINGS_G2_ORDER_MASK)>>
                                     CSL_VPS_COMP_SD_SETTINGS_G2_ORDER_SHIFT);
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Get priority for Video layer in the case g_reorder is
        * set to 0 */
        config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] =
                                    (VpsHal_CompDisplayOrder)((tempReg &
                                    CSL_VPS_COMP_SD_SETTINGS_VID_ORDER_MASK)>>
                                    CSL_VPS_COMP_SD_SETTINGS_VID_ORDER_SHIFT);
    }

    /* Get different i/p enabled or disbled */
    config->grpx0InEnable =
                      ((tempReg & CSL_VPS_COMP_SD_SETTINGS_GRPX0_EN_MASK) >>
                      CSL_VPS_COMP_SD_SETTINGS_GRPX0_EN_SHIFT);

    config->grpx1InEnable =
                      ((tempReg & CSL_VPS_COMP_SD_SETTINGS_GRPX1_EN_MASK) >>
                      CSL_VPS_COMP_SD_SETTINGS_GRPX1_EN_SHIFT);

    config->grpx2InEnable =
                      ((tempReg & CSL_VPS_COMP_SD_SETTINGS_GRPX2_EN_MASK) >>
                      CSL_VPS_COMP_SD_SETTINGS_GRPX2_EN_SHIFT);

    config->videoInConfig.enableSd =
                      ((tempReg & CSL_VPS_COMP_SD_SETTINGS_VID_EN_MASK) >>
                      CSL_VPS_COMP_SD_SETTINGS_VID_EN_SHIFT);


    return (0);
}



static UInt32 compConfigureDvo2(VpsHal_CompObj *compInfo,
                                const VpsHal_CompConfig *config)
{
    UInt32  tempReg = 0u;
    UInt32  dispCnt = 0u;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    tempReg = compInfo->dvo2Settings & VPSHAL_COMP_ENABLE_PATH_MASK;

    /* Set the alpha blending o/p -final/video */
    if(VPSHAL_COMP_OUT_FINAL_ALPHA_BLENDING == config->fbPath)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_FB_SEL_MASK;
    }

    /* Set the global re-order option */
    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_G_ORDER_MASK;

        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            /* Set the display/re order priority */
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_DVO2_SETTINGS_VID_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_DVO2_SETTINGS_G0_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_DVO2_SETTINGS_G1_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_DVO2_SETTINGS_G2_ORDER_SHIFT;
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Setting priority for Video layer in the case g_reordewr is
        * set to 0 */
        tempReg |= config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] <<
                           CSL_VPS_COMP_DVO2_SETTINGS_VID_ORDER_SHIFT;
    }

    /* Select different i/p to compsoitor /blender */

    if(TRUE == config->grpx0InEnable)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_GRPX0_EN_MASK;
    }
    if(TRUE == config->grpx1InEnable)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_GRPX1_EN_MASK;
    }
    if(TRUE == config->grpx2InEnable)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_GRPX2_EN_MASK;
    }

    if(TRUE == config->videoInConfig.hdmiConfig.enableHdVid)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_VID_EN_MASK;
    }

    if(TRUE == config->videoInConfig.hdmiConfig.enableHdPip)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_PIP_EN_MASK;
    }

    if(VPSHAL_COMP_DVO2_BLEND_BASE_HD_PIP==config->videoInConfig.dvo2Config.dvo2Base)
    {
        tempReg |= CSL_VPS_COMP_DVO2_SETTINGS_VID_BLD_ORDER_MASK;
    }

    return (tempReg);
}



static UInt32 compConfigureSD(VpsHal_CompObj *compInfo,
                              const VpsHal_CompConfig *config)
{
    UInt32  tempReg = 0u;
    UInt32  dispCnt = 0u;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    tempReg = compInfo->sdSettings & VPSHAL_COMP_ENABLE_PATH_MASK;

    /* Set the alpha blending o/p -final or video */
    if(VPSHAL_COMP_OUT_FINAL_ALPHA_BLENDING == config->fbPath)
    {
        tempReg |= CSL_VPS_COMP_SD_SETTINGS_FB_SEL_MASK;
    }

    /* Set the global re-order option */
    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        tempReg |= CSL_VPS_COMP_SD_SETTINGS_G_ORDER_MASK;

        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            /* Set the display/re order priority */
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_SD_SETTINGS_VID_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_SD_SETTINGS_G0_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_SD_SETTINGS_G1_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_SD_SETTINGS_G2_ORDER_SHIFT;
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Setting priority for Video layer in the case g_reordewr is
        * set to 0 */
        tempReg |= config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] <<
                           CSL_VPS_COMP_SD_SETTINGS_VID_ORDER_SHIFT;
    }

    /* Select different i/p to compsoitor /blender */

    if(TRUE == config->grpx0InEnable)
    {
        tempReg |= CSL_VPS_COMP_SD_SETTINGS_GRPX0_EN_MASK;
    }
    if(TRUE == config->grpx1InEnable)
    {
        tempReg |= CSL_VPS_COMP_SD_SETTINGS_GRPX1_EN_MASK;
    }
    if(TRUE == config->grpx2InEnable)
    {
        tempReg |= CSL_VPS_COMP_SD_SETTINGS_GRPX2_EN_MASK;
    }

    if(TRUE == config->videoInConfig.enableSd)
    {
        tempReg |= CSL_VPS_COMP_SD_SETTINGS_VID_EN_MASK;
    }

    return (tempReg);
}



static UInt32 compConfigureHDComp(VpsHal_CompObj *compInfo,
                                  const VpsHal_CompConfig *config)
{
    UInt32  tempReg = 0u;
    UInt32  dispCnt = 0u;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    tempReg = compInfo->hdcompSettings & VPSHAL_COMP_ENABLE_PATH_MASK;

    /* Set the alpha blending o/p -final or video */
    if(VPSHAL_COMP_OUT_FINAL_ALPHA_BLENDING == config->fbPath)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_FB_SEL_MASK;
    }

    /* Set the global re-order option */
    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_G_ORDER_MASK;

        /* Set the display/re order priority */
        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                                CSL_VPS_COMP_HDCOMP_SETTINGS_VID_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                                CSL_VPS_COMP_HDCOMP_SETTINGS_G0_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                                CSL_VPS_COMP_HDCOMP_SETTINGS_G1_ORDER_SHIFT;
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    tempReg |= config->displayOrder[dispCnt] <<
                                CSL_VPS_COMP_HDCOMP_SETTINGS_G2_ORDER_SHIFT;
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Setting priority for Video layer in the case g_reordewr is
        * set to 0 */
        tempReg |= config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] <<
                           CSL_VPS_COMP_HDCOMP_SETTINGS_VID_ORDER_SHIFT;
    }

    /* Select different i/p to compsoitor /blender */
    if(TRUE == config->grpx0InEnable)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX0_EN_MASK;
    }
    if(TRUE == config->grpx1InEnable)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX1_EN_MASK;
    }
    if(TRUE == config->grpx2InEnable)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_GRPX2_EN_MASK;
    }

    if(TRUE == config->videoInConfig.hdCompConfig.enableHdCit)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_VID_EN_MASK;
    }

    if(TRUE == config->videoInConfig.hdCompConfig.enableHdPip)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_PIP_EN_MASK;
    }

    if(VPSHAL_COMP_HDCOMP_BLEND_BASE_HD_PIP ==
       config->videoInConfig.hdCompConfig.hdCompBase)
    {
        tempReg |= CSL_VPS_COMP_HDCOMP_SETTINGS_VID_BLD_ORDER_MASK;
    }

    return (tempReg);

}

static UInt32 compConfigureHDMI(VpsHal_CompObj *compInfo,
                                const VpsHal_CompConfig *config)
{
    UInt32  tempReg = 0u;
    UInt32  dispCnt = 0u;

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != compInfo));
    GT_assert(VpsHalTrace, (NULL != config));

    tempReg = compInfo->hdmiSettings & VPSHAL_COMP_ENABLE_PATH_MASK;

    /* Set the alpha blending o/p -final or video */
    if(VPSHAL_COMP_OUT_FINAL_ALPHA_BLENDING == config->fbPath)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_FB_SEL_MASK;
    }

    /* Set the global re-order option */
    if(VPSHAL_COMP_GLOBAL_REORDER_ON == config->gReorderMode)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_G_ORDER_MASK;

        for(dispCnt = 0u; dispCnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; dispCnt ++)
        {
            /* Set the display/re order priority */
            switch((VpsHal_CompDisplayOrder)dispCnt)
            {
                case VPSHAL_COMP_DISPLAY_VID_ORDER :
                    tempReg |= (config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_SHIFT) &
                           CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_MASK;
                break;

                case VPSHAL_COMP_DISPLAY_G0_ORDER :
                    tempReg |= (config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_HDMI_SETTINGS_G0_ORDER_SHIFT) &
                           CSL_VPS_COMP_HDMI_SETTINGS_G0_ORDER_MASK;
                break;

                case VPSHAL_COMP_DISPLAY_G1_ORDER :
                    tempReg |= (config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_HDMI_SETTINGS_G1_ORDER_SHIFT) &
                           CSL_VPS_COMP_HDMI_SETTINGS_G1_ORDER_MASK;
                break;

                case VPSHAL_COMP_DISPLAY_G2_ORDER :
                    tempReg |= (config->displayOrder[dispCnt] <<
                           CSL_VPS_COMP_HDMI_SETTINGS_G2_ORDER_SHIFT) &
                           CSL_VPS_COMP_HDMI_SETTINGS_G2_ORDER_SHIFT;
                break;

                default:
                break;
            }
        }
    }
    else
    {
        /* Setting priority for Video layer in the case g_reordewr is
        * set to 0 */
        tempReg |= config->displayOrder[VPSHAL_COMP_DISPLAY_VID_ORDER] <<
                           CSL_VPS_COMP_HDMI_SETTINGS_VID_ORDER_SHIFT;
    }

    /* Select different i/p to compsoitor /blender */
    if(TRUE == config->grpx0InEnable)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_GRPX0_EN_MASK;
    }
    if(TRUE == config->grpx1InEnable)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_GRPX1_EN_MASK;
    }
    if(TRUE == config->grpx2InEnable)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_GRPX2_EN_MASK;
    }

    if(TRUE == config->videoInConfig.hdmiConfig.enableHdVid)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_VID_EN_MASK;
    }

    if(TRUE == config->videoInConfig.hdmiConfig.enableHdPip)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_PIP_EN_MASK;
    }

    if(VPSHAL_COMP_HDMI_BLEND_BASE_HD_PIP==config->videoInConfig.hdmiConfig.hdmiBase)
    {
        tempReg |= CSL_VPS_COMP_HDMI_SETTINGS_VID_BLD_ORDER_MASK;
    }

    return (tempReg);
}
