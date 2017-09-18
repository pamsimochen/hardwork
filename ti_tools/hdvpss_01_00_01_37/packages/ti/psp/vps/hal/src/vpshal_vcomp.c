/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_vcomp.c
 *
 * \brief VCOMP  HAL file.
 * This file implements the HAL APIs of the VPS VCOMP.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_ede.h>
#include <ti/psp/cslr/cslr_vps_vcomp.h>
#include <ti/psp/vps/hal/vpshal_vcomp.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum Comp_ConfigOvlyReg
 *  \brief Enums for the configuration overlay register offsets - used as array indices
 *  to register offset array in Vcomp_Obj object. These enums defines the
 *  registers that will be used to form the configuration register overlay for
 *  VCOMP module. This in turn is used by client drivers to configure registers
 *  using VPDMA during context-switching (software channels with different
 *  configurations).
 *
 */
typedef enum
{
    VCOMP_CONFIG_OVLY_REG0_IDX = 0,
    VCOMP_CONFIG_OVLY_REG1_IDX,
    VCOMP_CONFIG_OVLY_REG2_IDX,
    VCOMP_CONFIG_OVLY_REG3_IDX,
    VCOMP_CONFIG_OVLY_REG4_IDX,
    VCOMP_CONFIG_OVLY_REG5_IDX,
    VCOMP_CONFIG_OVLY_REG6_IDX,
    VCOMP_CONFIG_OVLY_REG7_IDX,
    VCOMP_CONFIG_OVLY_REG8_IDX,
    VCOMP_CONFIG_OVLY_REG9_IDX,
    VCOMP_CONFIG_OVLY_REG10_IDX,
    VCOMP_CONFIG_OVLY_REG11_IDX,
    VCOMP_CONFIG_OVLY_REG12_IDX,
    VCOMP_CONFIG_OVLY_NUM_IDX
    /**< This should be the last enum */
} Vcomp_ConfigOvlyReg;

typedef enum
{
    VCOMP_RTCONFIG_OVLY_REG0_IDX = 0,
    VCOMP_RTCONFIG_OVLY_REG1_IDX,
    VCOMP_RTCONFIG_OVLY_REG2_IDX,
    VCOMP_RTCONFIG_OVLY_REG7_IDX,
    VCOMP_RTCONFIG_OVLY_NUM_IDX
    /**< This should be the last enum */
} Vcomp_RtConfigOvlyReg;

typedef enum
{
    VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX = 0,
    VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX
    /**< This should be the last enum */
} Vcomp_EnableConfigOvlyReg;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */



/**
 *  struct Comp_Info
 *  \brief Structure containing per  instance information for each VCOMP.
 *
 */
typedef struct
{
    UInt32             instId;
    /**< VCOMP instance ID */
    UInt32             openCnt;
    /**< Counter to keep track of number of open calls for an instance */
    CSL_Vps_vcompRegs *regOvly;
    /**< Pointer to the DEI register overlay */
    UInt32             configOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay memory required
         COMP module */
    UInt32             mainConfigOvlySize;
    UInt32             auxConfigOvlySize;
    UInt32             mainEnableConfigOvlySize;
    UInt32             auxEnableConfigOvlySize;
    UInt32            *regOffset[VCOMP_CONFIG_OVLY_NUM_IDX];
    /**< Array of register offsets for all the registers that needs to be
         programmed through VPDMA.*/
    UInt32             virRegOffset[VCOMP_CONFIG_OVLY_NUM_IDX];
    /**< Array of virtual register offsets in the VPDMA register
         configuration overlay memory corresponding to regOffset member.*/

    UInt32            *mainRegOffset[VCOMP_RTCONFIG_OVLY_NUM_IDX];
    UInt32             mainVirRegOffset[VCOMP_RTCONFIG_OVLY_NUM_IDX];
    UInt32            *auxRegOffset[VCOMP_RTCONFIG_OVLY_NUM_IDX];
    UInt32             auxVirRegOffset[VCOMP_RTCONFIG_OVLY_NUM_IDX];

    UInt32            *mainEnableRegOffset[VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX];
    UInt32             mainEnableVirRegOffset[VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX];
    UInt32            *auxEnableRegOffset[VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX];
    UInt32             auxEnableVirRegOffset[VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX];

    /* VCOMP has to keep track of size */
    UInt32             mainWidth, mainHeight;
    UInt32             auxWidth, auxHeight;
} Vcomp_Obj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  This local API could be used to set only Aux paramters by setting
 *  dwnStrmEncodFid and dwnStrmSrcFid to NULL
 */
static Int vcompSetVideoAux(VpsHal_Handle handle,
                               const VpsHal_VcompInputPathConfig *auxConfig,
                               const VpsHal_VcompDwnStrmSrcFid *dwnStrmSrcFid,
                               const VpsHal_VcompDwnStrmEncFid *dwnStrmEncodFid,
                               Ptr configOvlyPtr);

/**
 * This local API could be used to set only Main paramters by setting
 * nfHandling to NULL
 */
static Int vcompSetVideoMain(VpsHal_Handle handle,
                               const VpsHal_VcompInputPathConfig *mainConfig,
                               const VpsHal_VcompNfHandling *nfHandling,
                               Ptr configOvlyPtr);

static Int vcompSetDisplayConfig(VpsHal_Handle handle,
                                 const VpsHal_VcompOutputDsplyConfig *outDsplyConfig,
                                 const UInt32 *timeout,
                                 Ptr configOvlyPtr);

/**
 * This local API could be used to get only Aux paramters by setting
 * dwnStrmEncodFid and dwnStrmSrcFid to NULL
 */
static Int vcompGetVideoAux(VpsHal_Handle handle,
                               VpsHal_VcompInputPathConfig *auxConfig,
                               VpsHal_VcompDwnStrmSrcFid *dwnStrmSrcFid,
                               VpsHal_VcompDwnStrmEncFid *dwnStrmEncodFid);

/**
 * This local API could be used to get only Main paramters by setting
 * nfHandling to NULL
 */
static Int vcompGetVideoMain(VpsHal_Handle handle,
                               VpsHal_VcompInputPathConfig *mainConfig,
                               VpsHal_VcompNfHandling *nfHandling);

static Int vcompGetDisplayConfig(VpsHal_Handle handle,
                                 VpsHal_VcompOutputDsplyConfig *outDsplyConfig,
                                 UInt32 *timeout);


/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */

/**
 *  VcompObjects
 *  \brief Module variable to store information about each VCOMP
 *  instance.
 *  Note: If the number of VCOMP instance increase, then this variable should be
 *  initialized with the added VCOMP instance information.
 */
static Vcomp_Obj VcompObjects[VPSHAL_VCOMP_MAX_INST];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/**
 *  VpsHal_vcompInit
 *  \brief VCOMP HAL init function.
 *
 *  Initializes VCOMP objects, gets the register overlay offsets for VCOMP
 *  registers.
 *  This function should be called before calling any of VCOMP HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of VCOMP HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompInit(UInt32 numInstances,
                     VpsHal_VcompInstParams *vcompInstParams,
                     Ptr arg)
{
    Int                 instCnt;
    Vcomp_Obj           *vcompObj;
    CSL_Vps_vcompRegs   *regOvly;
    Int                 ret     = 0;

    GT_assert( GT_DEFAULT_MASK, NULL != vcompInstParams);
    GT_assert( GT_DEFAULT_MASK, numInstances <= VPSHAL_VCOMP_MAX_INST);

    VpsUtils_memset(VcompObjects, 0, sizeof(VcompObjects));
    for (instCnt = 0; instCnt < numInstances; instCnt++)
    {
        GT_assert( GT_DEFAULT_MASK, vcompInstParams[instCnt].instId < VPSHAL_VCOMP_MAX_INST);

        vcompObj = &VcompObjects[instCnt];

        vcompObj->instId = vcompInstParams[instCnt].instId;
        vcompObj->regOvly =
                (CSL_Vps_vcompRegs *)vcompInstParams[instCnt].baseAddress;

        regOvly = vcompObj->regOvly;

        /* Initialize the register offsets for the registers which are used
         * to form the VPDMA configuration overlay */
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG0_IDX] =
                                            (UInt32 *) &regOvly->REG0;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG1_IDX] =
                                            (UInt32 *) &regOvly->REG1;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG2_IDX] =
                                            (UInt32 *) &regOvly->REG2;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG3_IDX] =
                                            (UInt32 *) &regOvly->REG3;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG4_IDX] =
                                            (UInt32 *) &regOvly->REG4;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG5_IDX] =
                                            (UInt32 *) &regOvly->REG5;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG6_IDX] =
                                            (UInt32 *) &regOvly->REG6;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG7_IDX] =
                                            (UInt32 *) &regOvly->REG7;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG8_IDX] =
                                            (UInt32 *) &regOvly->REG8;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG9_IDX] =
                                            (UInt32 *) &regOvly->REG9;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG10_IDX] =
                                            (UInt32 *) &regOvly->REG10;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG11_IDX] =
                                            (UInt32 *) &regOvly->REG11;
        vcompObj->regOffset[VCOMP_CONFIG_OVLY_REG12_IDX] =
                                            (UInt32 *) &regOvly->REG12;

        /* Get the size of the overlay for VCOMP registers and the relative
         * virtual offset for the above registers when VPDMA config register
         * overlay is formed */
        vcompObj->configOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                      vcompObj->regOffset,
                                      VCOMP_CONFIG_OVLY_NUM_IDX,
                                      vcompObj->virRegOffset);

        vcompObj->mainEnableRegOffset[VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX] =
                                            (UInt32 *) &regOvly->REG0;
        vcompObj->mainEnableConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                      vcompObj->mainEnableRegOffset,
                                      VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX,
                                      vcompObj->mainEnableVirRegOffset);

        vcompObj->auxEnableRegOffset[VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX] =
                                            (UInt32 *) &regOvly->REG3;
        vcompObj->auxEnableConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                      vcompObj->auxEnableRegOffset,
                                      VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX,
                                      vcompObj->auxEnableVirRegOffset);
        /* Initialize the register offsets for the registers which are used
         * to form the VPDMA configuration overlay */
        vcompObj->mainRegOffset[VCOMP_RTCONFIG_OVLY_REG0_IDX] =
                                            (UInt32 *) &regOvly->REG0;
        vcompObj->mainRegOffset[VCOMP_RTCONFIG_OVLY_REG1_IDX] =
                                            (UInt32 *) &regOvly->REG1;
        vcompObj->mainRegOffset[VCOMP_RTCONFIG_OVLY_REG2_IDX] =
                                            (UInt32 *) &regOvly->REG2;
        vcompObj->mainRegOffset[VCOMP_RTCONFIG_OVLY_REG7_IDX] =
                                            (UInt32 *) &regOvly->REG7;
        vcompObj->mainConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                      vcompObj->mainRegOffset,
                                      VCOMP_RTCONFIG_OVLY_NUM_IDX,
                                      vcompObj->mainVirRegOffset);

        vcompObj->auxRegOffset[VCOMP_RTCONFIG_OVLY_REG0_IDX] =
                                            (UInt32 *) &regOvly->REG3;
        vcompObj->auxRegOffset[VCOMP_RTCONFIG_OVLY_REG1_IDX] =
                                            (UInt32 *) &regOvly->REG4;
        vcompObj->auxRegOffset[VCOMP_RTCONFIG_OVLY_REG2_IDX] =
                                            (UInt32 *) &regOvly->REG5;
        vcompObj->auxRegOffset[VCOMP_RTCONFIG_OVLY_REG7_IDX] =
                                            (UInt32 *) &regOvly->REG8;
        vcompObj->auxConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                      vcompObj->auxRegOffset,
                                      VCOMP_RTCONFIG_OVLY_NUM_IDX,
                                      vcompObj->auxVirRegOffset);

        /* Set the background color */
        regOvly->REG9 |= (VPS_CFG_VCOMP_BKCOLOR & 0x3FFFFFFFu);
    }

    return (ret);
}



/**
 *  VpsHal_vcompDeInit
 *  \brief VCOMP HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_vcompOpen
 *  \brief Returns the handle to the requested VCOMP instance.
 *
 *  This function should be called prior to calling any of the VCOMP HAL
 *  configuration APIs to get the instance handle.
 *
 *  Vcomp_init should be called prior to this.
 *
 *  \param vcompInst Requested VCOMP instance.
 *
 *  \return          Returns VCOMP instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_vcompOpen(UInt32 vcompInst)
{
    Int         cnt;
    UInt32      cookie;
    VpsHal_Handle handle;


    handle = NULL;

    for (cnt = 0; cnt < VPSHAL_VCOMP_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (vcompInst == VcompObjects[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0 == VcompObjects[cnt].openCnt)
            {
                handle = (VpsHal_Handle) &VcompObjects[cnt];
                VcompObjects[cnt].openCnt++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }
    return (handle);
}




/**
 *  VpsHal_vcompClose
 *  \brief Closes the VCOMP HAL instance
 *
 *  Vcomp_init and Comp_open should be called prior to this.
 *
 *  \param handle Valid handle returned by Vcomp_open function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompClose(VpsHal_Handle handle)
{
    Int         ret     = -1;
    UInt32      cookie;
    Vcomp_Obj   *vcompObj;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    vcompObj = (Vcomp_Obj *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (vcompObj->openCnt > 0)
    {
        vcompObj->openCnt--;
        ret = 0;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (ret);
}



/**
 *  VpsHal_vcompGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA
 *  register configuration overlay required to program the VCOMP registers using
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
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  \param handle     Valid handle returned by Vcomp_open function.
 *  \param inputVideo NONE
 *
 *  \return           Returns 0 on success else returns error value.
 */
UInt32 VpsHal_vcompGetConfigOvlySize(VpsHal_Handle handle,
                                     VpsHal_VcompInputVideo inputVideo)
{
    UInt32 configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    /* Return the value already got in init function */
    if (VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        configOvlySize = ((Vcomp_Obj*) handle)->mainConfigOvlySize;
    }
    else
    {
        configOvlySize = ((Vcomp_Obj*) handle)->auxConfigOvlySize;
    }

    return (configOvlySize);
}



Int32 VpsHal_vcompGetEnableConfigOvlySize(VpsHal_Handle handle,
                                          VpsHal_VcompInputVideo inputVideo)
{
    UInt32 configOvlySize = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    /* Return the value already got in init function */
    if (VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        configOvlySize = ((Vcomp_Obj*) handle)->mainEnableConfigOvlySize;
    }
    else
    {
        configOvlySize = ((Vcomp_Obj*) handle)->auxEnableConfigOvlySize;
    }

    return (configOvlySize);
}



 /**
 *  VpsHal_vcompCreateConfigOvly
 *  \brief Creates the VCOMP register configuration
 *  overlay using VPDMA helper function.
 *
 *  This function does not initialize the overlay with VCOMP configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  Comp_setConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param inputVideo    NONE
 *  \param configOvlyPtr Pointer to the overlay memory where the overlay is
 *                       formed. The memory for the overlay should be allocated
 *                       by client driver. This parameter should be non-NULL.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompCreateConfigOvly(VpsHal_Handle handle,
                                 VpsHal_VcompInputVideo inputVideo,
                                 Ptr configOvlyPtr)
{
    Int ret=0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != configOvlyPtr);

    /* Create the register overlay */
    if (VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        ret = VpsHal_vpdmaCreateRegOverlay(
              ((Vcomp_Obj*) handle)->mainRegOffset,
              VCOMP_RTCONFIG_OVLY_NUM_IDX,
              configOvlyPtr);
    }
    else /* Aux Video */
    {
        ret = VpsHal_vpdmaCreateRegOverlay(
              ((Vcomp_Obj*) handle)->auxRegOffset,
              VCOMP_RTCONFIG_OVLY_NUM_IDX,
              configOvlyPtr);
    }

    return (ret);
}



Int VpsHal_vcompCreateEnableConfigOvly(VpsHal_Handle handle,
                                       VpsHal_VcompInputVideo inputVideo,
                                       Ptr configOvlyPtr)
{
    Int ret=0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != configOvlyPtr);

    if (VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        /* Create the register overlay */
        ret = VpsHal_vpdmaCreateRegOverlay(
              ((Vcomp_Obj*) handle)->mainEnableRegOffset,
              VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX,
              configOvlyPtr);
    }
    else
    {
        /* Create the register overlay */
        ret = VpsHal_vpdmaCreateRegOverlay(
              ((Vcomp_Obj*) handle)->auxEnableRegOffset,
              VCOMP_ENABLE_CONFIG_OVLY_NUM_IDX,
              configOvlyPtr);
    }

    return (ret);
}



Int VpsHal_vcompEnableVideoInputPath(VpsHal_Handle handle,
                                     VpsHal_VcompInputVideo inputVideo,
                                     UInt32 enable,
                                     Ptr configOvlyPtr)
{
    Vcomp_Obj              *vcompObj = NULL;
    UInt32                 *ovlyPtr = NULL;
    CSL_Vps_vcompRegs      *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    if (NULL == configOvlyPtr)
    {
        if (VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
        {
            regOvly->REG0 &= (~CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_MASK);
            if (TRUE == enable)
            {
                regOvly->REG0 |= CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_MASK;
            }
        }
        else
        {
            regOvly->REG3 &= (~CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_MASK);
            if (TRUE == enable)
            {
                regOvly->REG3 |= CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_MASK;
            }
        }
    }
    else
    {

        if (VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
        {
            ovlyPtr = (UInt32 *)configOvlyPtr + vcompObj->mainEnableVirRegOffset[VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX];
            (*ovlyPtr) &= (~CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_MASK);
            if (TRUE == enable)
            {
                (*ovlyPtr) |= CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_MASK;
            }
        }
        else
        {
            ovlyPtr = (UInt32 *)configOvlyPtr + vcompObj->
                    auxEnableVirRegOffset[VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX];
            (*ovlyPtr) &= (~CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_MASK);
            if (TRUE == enable)
            {
                (*ovlyPtr) |= CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_MASK;
            }
        }
    }
    return (0);
}



Int VpsHal_vcompSetInputSize(VpsHal_Handle handle,
                             VpsHal_VcompInputVideo inputVideo,
                             UInt16 width,
                             UInt16 height,
                             Ptr configOvlyPtr)
{
    Vcomp_Obj              *vcompObj = NULL;
    UInt32                 *ovlyPtr = NULL;
    CSL_Vps_vcompRegs      *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    if(VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        /* Configure size for the main window */
        if (NULL == configOvlyPtr)
        {
            regOvly->REG0 &=
                ~(CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_MASK|
                  CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_MASK);
            regOvly->REG0 |= (((width <<
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_SHIFT) &
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_MASK) |
                    ((height <<
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_SHIFT) &
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_MASK));
        }
        else /* Configure size in the overlay memory */
        {
            ovlyPtr = (UInt32 *)configOvlyPtr + vcompObj->
                    mainEnableVirRegOffset[VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX];
            (*ovlyPtr) &=
                ~(CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_MASK|
                  CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_MASK);
            (*ovlyPtr) |= (((width <<
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_SHIFT) &
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_MASK) |
                    ((height <<
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_SHIFT) &
                    CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_MASK));
        }
        vcompObj->mainWidth = width;
        vcompObj->mainHeight = height;
    }
    else /* Configure size for the aux window */
    {
        /* Configure size for the main window */
        if (NULL == configOvlyPtr)
        {
            regOvly->REG3 &=
                ~(CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_MASK|
                  CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_MASK);
            regOvly->REG3 |= (((width <<
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_SHIFT) &
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_MASK) |
                    ((height <<
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_SHIFT) &
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_MASK));
        }
        else /* Configure size in the overlay memory */
        {
            ovlyPtr = (UInt32 *)configOvlyPtr + vcompObj->
                    auxEnableVirRegOffset[VCOMP_ENABLE_CONFIG_OVLY_REG0_IDX];
            (*ovlyPtr) &=
                ~(CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_MASK|
                  CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_MASK);
            (*ovlyPtr) |= (((width <<
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_SHIFT) &
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_MASK) |
                    ((height <<
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_SHIFT) &
                    CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_MASK));
        }
        vcompObj->auxWidth = width;
        vcompObj->auxHeight = height;
    }
    return (0);
}



Void VpsHal_vcompGetInputSize(VpsHal_Handle handle,
                              VpsHal_VcompInputVideo inputVideo,
                              UInt32 *width,
                              UInt32 *height)
{
    if(VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        *width = ((Vcomp_Obj *)handle)->mainWidth;
        *height = ((Vcomp_Obj *)handle)->mainHeight;
    }
    else
    {
        *width = ((Vcomp_Obj *)handle)->auxWidth;
        *height = ((Vcomp_Obj *)handle)->auxHeight;
    }
}
/**
 *  VpsHal_vcompSetConfig
 *  \brief Sets the entire VCOMP configuration to either the
 *  actual VCOMP registers or to the configuration overlay memory.
 *
 *  This function configures the entire VCOMP registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual VCOMP MMR or
 *  to configuration overlay memory.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  VCOMP registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param config        Pointer to VpsHal_VcompConfig structure containing the
 *                       register configurations. This parameter should be
 *                       non-NULL.
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual VCOMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompSetConfig(VpsHal_Handle handle,
                   const VpsHal_VcompConfig *config,
                   Ptr configOvlyPtr)
{
    Int   ret     = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != config);

    /* Set Aux input path paramters */
    ret= vcompSetVideoAux(handle,
                          &(config->auxVideo),
                          &(config->dwnStrmSrcFid),
                          &(config->dwnStrmEncoderFid),
                          configOvlyPtr);

    if(0 == ret)
    {
        /* Set Main input path paramters */
        ret= vcompSetVideoMain(handle,
                               &(config->mainVideo),
                               &(config->nfHandling),
                               configOvlyPtr);
    }

    if(0 == ret)
    {
        /* Set display o/p path paramters */
        ret= vcompSetDisplayConfig(handle,
                                   &(config->dsplyConfig),
                                   &(config->dsplyTimeout),
                                   configOvlyPtr);
    }
    return (ret);
}



/**
 *  VpsHal_vcompGetConfig
 *  \brief Gets the entire VCOMP configuration from the actual
 *  VCOMP registers.
 *
 *  This function gets the entire VCOMP configuration.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  \param handle Valid handle returned by Comp_open function.
 *
 *  \param config Pointer to VpsHal_VcompConfig structure to be filled with
 *                register configurations. This parameter should be non-NULL.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompGetConfig(VpsHal_Handle handle,
                   VpsHal_VcompConfig *config)
{

    Int   ret     = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != config);

    /* Get Aux input path paramters */
    ret= vcompGetVideoAux(handle,
                          &(config->auxVideo),
                          &(config->dwnStrmSrcFid),
                          &(config->dwnStrmEncoderFid));

    if(0 == ret)
    {
        /* Get Main input path paramters */
        ret= vcompGetVideoMain(handle,
                               &(config->mainVideo),
                               &(config->nfHandling));
    }

    if(0 == ret)
    {
        /* Get display o/p path paramters */
        ret= vcompGetDisplayConfig(handle,
                                   &(config->dsplyConfig),
                                   &(config->dsplyTimeout));
    }

    return (ret);
}



/**
 *  VpsHal_vcompSetInputVideoConfig
 *  \brief Set the input configuration for either
 *  aux or main video of VCOMP to either the actual VCOMP registers or to the
 *  configuration overlay memory.
 *
 *  This function configures only aux or main input channel.Depending on the
 *  value of configOvlyPtr parameter, the updating will happen to actual VCOMP
 *  MMR or to configuration overlay memory.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  Aux oe Main input configuration registers are programmed
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param inputVideo    NONE
 *
 *  \param inVideoconfig Pointer to VpsHal_VcompInputPathConfig structure
 *                       containing the register configurations for either Aux
 *                       or Main input video paramters. This parameter should be
 *                       non-NULL.
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual VCOMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompSetInputVideoConfig(VpsHal_Handle handle,
                              VpsHal_VcompInputVideo inputVideo,
                              const VpsHal_VcompInputPathConfig *inVideoconfig,
                              Ptr configOvlyPtr)
{
    Int   ret     = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != inVideoconfig);

    if(VPSHAL_VCOMP_SOURCE_AUX == inputVideo)
    {
        /* Set Aux input path paramters */
        ret= vcompSetVideoAux(handle,
                          inVideoconfig,
                          NULL,
                          NULL,
                          configOvlyPtr);
    }

    if(VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        /* Set Main input path paramters */
        ret= vcompSetVideoMain(handle,
                               inVideoconfig,
                               NULL,
                               configOvlyPtr);
    }
    return (ret);
}


/**
 *  VpsHal_vcompGetInputVideoConfig
 *  \brief Get the input configuration for either
 *  aux or main video of VCOMP.
 *
 *  This function get configures for only aux or main input channel.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  Aux or Main input configuration registers are returned
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param inputVideo    NONE
 *
 *  \param inVideoconfig Pointer to VpsHal_VcompInputPathConfig structure
 *                       returns configurations for either Aux or Main input
 *                       video paramters. This parameter should be non-NULL.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompGetInputVideoConfig(VpsHal_Handle handle,
                              VpsHal_VcompInputVideo inputVideo,
                              VpsHal_VcompInputPathConfig *inVideoconfig)
{
    Int   ret     = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != inVideoconfig);

    if(VPSHAL_VCOMP_SOURCE_AUX == inputVideo)
    {
        /* Get Aux input path paramters */
        ret= vcompGetVideoAux(handle,
                          inVideoconfig,
                          NULL,
                          NULL);
    }

    if(VPSHAL_VCOMP_SOURCE_MAIN == inputVideo)
    {
        /* Get Main input path paramters */
        ret= vcompGetVideoMain(handle,
                               inVideoconfig,
                               NULL);
    }

    return (ret);
}


/**
 *  VpsHal_vcompSetOutputDisplayConfig
 *  \brief Set display output configuration
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  display output configuration registers are programmed
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param displyconfig  Pointer to VpsHal_VcompOutputDsplyConfig structure
 *                       containing the register configurations for Vcomp output
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual VCOMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompSetOutputDisplayConfig(VpsHal_Handle handle,
                              const VpsHal_VcompOutputDsplyConfig *displyconfig,
                              Ptr configOvlyPtr)
{
    Int   ret     = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != displyconfig);

    /* Set display o/p path paramters */
    ret= vcompSetDisplayConfig(handle,
                               displyconfig,
                               NULL,
                               configOvlyPtr);
    return (ret);
}


/**
 *  VpsHal_vcompGetOutputDisplayConfig
 *  \brief Get the input configuration for either aux or main video of VCOMP.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  Aux or Main input configuration registers are returned
 *
 *  \param handle       Valid handle returned by Vcomp_open function.
 *
 *  \param displyconfig Pointer to VpsHal_VcompOutputDsplyConfig structure
 *                      returns configurations for  for Vcomp output.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_vcompGetOutputDisplayConfig(VpsHal_Handle handle,
                              VpsHal_VcompOutputDsplyConfig *displyconfig)
{
        Int   ret     = 0;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != displyconfig);

     ret= vcompGetDisplayConfig(handle,
                                displyconfig,
                                NULL);
    return (ret);
}


static Int vcompGetVideoAux(VpsHal_Handle handle,
                               VpsHal_VcompInputPathConfig *auxConfig,
                               VpsHal_VcompDwnStrmSrcFid *dwnStrmSrcFid,
                               VpsHal_VcompDwnStrmEncFid *dwnStrmEncodFid)
{
    Int                    ret = 0;
    CSL_Vps_vcompRegs      *regOvly;
    UInt32                 tempReg3, tempReg4, tempReg5, tempReg8, tempReg12;
    Vcomp_Obj              *vcompObj;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    /*Set to zero*/
    tempReg3= tempReg4= tempReg5= tempReg8= tempReg12=0;

    /* Read REG 3 register */
    tempReg3 = regOvly->REG3;
    /* Get enable status for main video path */
    auxConfig->enabled = (( tempReg3 &
                          CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_MASK) >>
                          CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_SHIFT);
     /* get enable status for alternate color */
    auxConfig->enableFixedData = (( tempReg3 &
                          CSL_VPS_VCOMP_REG3_CFG_AUX_FIXED_DATA_SEND_MASK) >>
                          CSL_VPS_VCOMP_REG3_CFG_AUX_FIXED_DATA_SEND_SHIFT);
    /* get number of lines in a field or frame from the incoming main source */
    auxConfig->numLines = (( tempReg3 &
                          CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_MASK) >>
                          CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_SHIFT);
    /* get number of pixels per line from the incoming main source */
    auxConfig->numPix = (( tempReg3 &
              CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_MASK) >>
              CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_SHIFT);

    /* Read REG 4 register */
    tempReg4 = regOvly->REG4;
    /* Get  number of incoming pixels to discard as part of the Compositor's
     * picture clipping feature */
    auxConfig->clipWindow.skipNumPix = (( tempReg4 &
               CSL_VPS_VCOMP_REG4_CFG_AUX_SKIP_NUMPIX_MASK) >>
               CSL_VPS_VCOMP_REG4_CFG_AUX_SKIP_NUMPIX_SHIFT);
    /* Get number of pixels to use from each incoming line */
    auxConfig->clipWindow.useNumPix = (( tempReg4 &
               CSL_VPS_VCOMP_REG4_CFG_AUX_USE_NUMPIX_MASK) >>
               CSL_VPS_VCOMP_REG4_CFG_AUX_USE_NUMPIX_SHIFT);
    /*Get start up condition */
    auxConfig->startUpHandling =  (VpsHal_VcompStartUpHandling)(( tempReg4 &
                      CSL_VPS_VCOMP_REG4_CFG_AUX_STARTUP_HANDLING_MASK) >>
                      CSL_VPS_VCOMP_REG4_CFG_AUX_STARTUP_HANDLING_SHIFT);

    /* Read REG 5 register */
    tempReg5 = regOvly->REG5;
    /* Get   number of incoming lines to discard as part of the Compositor's
     * picture clipping feature */
    auxConfig->clipWindow.skipNumLines = ((tempReg5 &
               CSL_VPS_VCOMP_REG5_CFG_AUX_SKIP_NUMLINES_MASK) >>
               CSL_VPS_VCOMP_REG5_CFG_AUX_SKIP_NUMLINES_SHIFT);
    /*  Get number of lines to use from each incoming field or frame */
    auxConfig->clipWindow.useNumLines = ((tempReg5 &
               CSL_VPS_VCOMP_REG5_CFG_AUX_USE_NUMLINES_MASK ) >>
               CSL_VPS_VCOMP_REG5_CFG_AUX_USE_NUMLINES_SHIFT);

    /* Read REG 8 register */
    tempReg8 = regOvly->REG8;
    /* Get Row origin index for the Main source picture */
    auxConfig->dsplyPos.yOrigin = ((tempReg8 &
               CSL_VPS_VCOMP_REG8_CFG_DSPLY_AUX_Y_ORIGIN_MASK) >>
               CSL_VPS_VCOMP_REG8_CFG_DSPLY_AUX_Y_ORIGIN_SHIFT);
    /* Get column origin index for the Main source picture */
    auxConfig->dsplyPos.xOrigin = ((tempReg8 &
               CSL_VPS_VCOMP_REG8_CFG_DSPLAY_AUX_X_ORIGIN_MASK) >>
               CSL_VPS_VCOMP_REG8_CFG_DSPLAY_AUX_X_ORIGIN_SHIFT);
     /*Get src down stream Fid*/
     if(NULL != dwnStrmSrcFid)
     {
         *dwnStrmSrcFid = (VpsHal_VcompDwnStrmSrcFid) ((tempReg8 &
               CSL_VPS_VCOMP_REG8_CFG_DWNSTRM_SRC_FID_CTRL_MASK) >>
               CSL_VPS_VCOMP_REG8_CFG_DWNSTRM_SRC_FID_CTRL_SHIFT);
     }
     /* Read REG 12 register */
    tempReg12 = regOvly->REG12;

    /* Get Alternate Main Y/Luma value for the background display output */
    auxConfig->altColor.yLuma = (( tempReg12 &
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_Y_VAL_MASK) >>
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_Y_VAL_SHIFT);
    /* Get Alternate Main Cr/Chroma value for the background display output */
    auxConfig->altColor.crChroma = ((tempReg12 &
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_CR_VAL_MASK) >>
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_CR_VAL_SHIFT);
    /* Get Alternate Main Cr/Chroma value for the background display output */
    auxConfig->altColor.cbChroma = ((tempReg12&
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_CB_VAL_MASK) >>
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_CB_VAL_SHIFT);

    /*Set encoder down stream Fid*/
     if(NULL != dwnStrmSrcFid)
     {
         *dwnStrmEncodFid =(VpsHal_VcompDwnStrmEncFid) ((tempReg12 &
               CSL_VPS_VCOMP_REG12_CFG_DWNSTRM_ENC_FID_CTRL_MASK) >>
               CSL_VPS_VCOMP_REG12_CFG_DWNSTRM_ENC_FID_CTRL_SHIFT);
     }
     return (ret);
}



static Int vcompGetVideoMain(VpsHal_Handle handle,
                               VpsHal_VcompInputPathConfig *mainConfig,
                               VpsHal_VcompNfHandling *nfHandling)
{
    Int                    ret = 0;
    CSL_Vps_vcompRegs      *regOvly;
    UInt32                 tempReg0, tempReg1, tempReg2, tempReg7, tempReg11;
    Vcomp_Obj              *vcompObj;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != mainConfig);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    /* Read REG 0 register */
    tempReg0 = regOvly->REG0;
    /* get enable status for main video path */
    mainConfig->enabled = (( tempReg0 &
                          CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_MASK) >>
                          CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_SHIFT);
    /* get enable status for alternate color */
    mainConfig->enableFixedData = (( tempReg0 &
                          CSL_VPS_VCOMP_REG0_CFG_MAIN_FIXED_DATA_SEND_MASK) >>
                          CSL_VPS_VCOMP_REG0_CFG_MAIN_FIXED_DATA_SEND_SHIFT);
    /* get number of lines in a field or frame from the incoming main source */
    mainConfig->numLines = (( tempReg0 &
                         CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_MASK) >>
                         CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_SHIFT);
    /* set number of pixels per line from the incoming main source */
    mainConfig->numPix = ((tempReg0 &
              CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_MASK) >>
              CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_SHIFT);

    /* Read REG 1 register */
    tempReg1 = regOvly->REG1;
    /* Get  number of incoming pixels to discard as part of the Compositor's
     * picture clipping feature */
    mainConfig->clipWindow.skipNumPix = (( tempReg1 &
                        CSL_VPS_VCOMP_REG1_CFG_MAIN_SKIP_NUMPIX_MASK) >>
                        CSL_VPS_VCOMP_REG1_CFG_MAIN_SKIP_NUMPIX_SHIFT);
    /* Get number of pixels to use from each incoming line */
    mainConfig->clipWindow.useNumPix = (( tempReg1 &
                         CSL_VPS_VCOMP_REG1_CFG_MAIN_USE_NUMPIX_MASK) >>
                         CSL_VPS_VCOMP_REG1_CFG_MAIN_USE_NUMPIX_SHIFT);
    /*Get start up condition */
    mainConfig->startUpHandling = (VpsHal_VcompStartUpHandling) (( tempReg1 &
                         CSL_VPS_VCOMP_REG1_CFG_MAIN_STARTUP_HANDLING_MASK) >>
                         CSL_VPS_VCOMP_REG1_CFG_MAIN_STARTUP_HANDLING_SHIFT);

    /* Read REG 2 register */
    tempReg2 = regOvly->REG2;
    /* Get   number of incoming lines to discard as part of the Compositor's
     * picture clipping feature */
    mainConfig->clipWindow.skipNumLines = (( tempReg2 &
               CSL_VPS_VCOMP_REG2_CFG_MAIN_SKIP_NUMLINES_MASK) >>
               CSL_VPS_VCOMP_REG2_CFG_MAIN_SKIP_NUMLINES_SHIFT);
    /*  Get number of lines to use from each incoming field or frame */
    mainConfig->clipWindow.useNumLines = (( tempReg2 &
               CSL_VPS_VCOMP_REG2_CFG_MAIN_USE_NUMLINES_MASK) >>
               CSL_VPS_VCOMP_REG2_CFG_MAIN_USE_NUMLINES_SHIFT);

    /* Read REG 7 register */
    tempReg7 = regOvly->REG7;
    /* Get Row origin index for the Main source picture */
    mainConfig->dsplyPos.yOrigin = (( tempReg7 &
               CSL_VPS_VCOMP_REG7_CFG_DSPLY_MAIN_Y_ORIGIN_MASK) >>
               CSL_VPS_VCOMP_REG7_CFG_DSPLY_MAIN_Y_ORIGIN_SHIFT);
    /* Get column origin index for the Main source picture */
    mainConfig->dsplyPos.xOrigin = ((tempReg7 &
               CSL_VPS_VCOMP_REG7_CFG_DSPLAY_MAIN_X_ORIGIN_MASK) >>
               CSL_VPS_VCOMP_REG7_CFG_DSPLAY_MAIN_X_ORIGIN_SHIFT);

    /* Read REG 7 register */
    tempReg11 = regOvly->REG11;
    mainConfig->altColor.yLuma = ((tempReg11 &
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_Y_VAL_MASK) >>
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_Y_VAL_SHIFT);
    /* Get Alternate Main Cr/Chroma value for the background display output */
    mainConfig->altColor.crChroma = (( tempReg11 &
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_CR_VAL_MASK) >>
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_CR_VAL_SHIFT);
    /* Get Alternate Main Cr/Chroma value for the background display output */
    mainConfig->altColor.cbChroma = ((tempReg11 &
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_CB_VAL_MASK) >>
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_CB_VAL_SHIFT);

    if(NULL != nfHandling)
    {
      *nfHandling = ( VpsHal_VcompNfHandling)(( tempReg11 &
               CSL_VPS_VCOMP_REG11_CFG_NF_HANDLING_MASK) >>
               CSL_VPS_VCOMP_REG11_CFG_NF_HANDLING_SHIFT);
    }

    return (ret);
}



static Int vcompGetDisplayConfig(VpsHal_Handle handle,
                                 VpsHal_VcompOutputDsplyConfig *outDsplyConfig,
                                 UInt32 *timeout)
{
    Int                    ret = 0;
    CSL_Vps_vcompRegs      *regOvly;
    UInt32                 tempReg9, tempReg6;
    Vcomp_Obj              *vcompObj;



    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != outDsplyConfig);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;


    tempReg9 = tempReg6 = 0;

    /* Read REG 6 register */
    tempReg6 = regOvly->REG6;

    /* get number of lines in a field or frame from the incoming main source */
    outDsplyConfig->dsplyNumlines = ((tempReg6 &
                                CSL_VPS_VCOMP_REG6_CFG_DSPLY_NUMLINES_MASK) >>
                                CSL_VPS_VCOMP_REG6_CFG_DSPLY_NUMLINES_SHIFT);

    /* set number of pixels per line from the incoming main source */
    outDsplyConfig->dsplyNumPix = ((tempReg6 &
                        CSL_VPS_VCOMP_REG6_CFG_DSPLY_NUMPIX_PER_LINE_MASK) >>
                        CSL_VPS_VCOMP_REG6_CFG_DSPLY_NUMPIX_PER_LINE_SHIFT);

      /* Read REG9 register */
    tempReg9 = regOvly->REG9;

    /* Get Y/Luma value for the background display output */
    outDsplyConfig->bckGrndColor.yLuma = (( tempReg9 &
                          CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_Y_VAL_MASK) >>
                          CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_Y_VAL_SHIFT);
    /* Get  Cr/Chrom10 value for10the background display output */
    outDsplyConfig->bckGrndColor.crChroma = (( tempReg9 &
                          CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_CR_VAL_MASK) >>
                          CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_CR_VAL_SHIFT);
    /* Get  Cr/Chroma value for the 9ckground display output */
    outDsplyConfig->bckGrndColor.cbChroma = (( tempReg9 &
                          CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_CB_VAL_MASK) >>
                          CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_CB_VAL_SHIFT);

    /* Get Video on top */
    outDsplyConfig->selectVideoOntop = (VpsHal_VcompInputVideo)(( tempReg9 &
                          CSL_VPS_VCOMP_REG9_CFG_MAIN_AUX_N_ONTOP_MASK) >>
                          CSL_VPS_VCOMP_REG9_CFG_MAIN_AUX_N_ONTOP_SHIFT);

    /* Get timeout */
    if( NULL != timeout)
    {
        *timeout = regOvly->REG10;
    }
    return (ret);
}
static Int vcompSetDisplayConfig(VpsHal_Handle handle,
                                 const VpsHal_VcompOutputDsplyConfig *outDsplyConfig,
                                 const UInt32 *timeout,
                                 Ptr configOvlyPtr)
{
    Int                    ret = 0;
    CSL_Vps_vcompRegs      *regOvly;
    UInt32                 tempReg9, tempReg10, tempReg6;
    Vcomp_Obj              *vcompObj;
    UInt32                 *virRegOffset;


    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != outDsplyConfig);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    /* Set to 0 */
    tempReg9 = tempReg10 = tempReg6 = 0;


    /* Configure REG 6 regiter */
    /* set number of lines in a field or frame from the incoming main source */
    tempReg6 |= outDsplyConfig->dsplyNumlines <<
              CSL_VPS_VCOMP_REG6_CFG_DSPLY_NUMLINES_SHIFT;

    /* set number of pixels per line from the incoming main source */
    tempReg6 |= outDsplyConfig->dsplyNumPix <<
              CSL_VPS_VCOMP_REG6_CFG_DSPLY_NUMPIX_PER_LINE_SHIFT;

    /* Configure REG 9 */
    /* Set Y/Luma value for the background display output */
    tempReg9 |= outDsplyConfig->bckGrndColor.yLuma <<
               CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_Y_VAL_SHIFT;
    /* Set  Cr/Chroma value for the background display output */
    tempReg9 |= outDsplyConfig->bckGrndColor.crChroma <<
               CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_CR_VAL_SHIFT;
    /* Set  Cr/Chroma value for the background display output */
    tempReg9 |= outDsplyConfig->bckGrndColor.cbChroma <<
               CSL_VPS_VCOMP_REG9_CFG_DSPLY_BCKGRND_CB_VAL_SHIFT;
    /* Select Video on top */
    if(VPSHAL_VCOMP_SOURCE_MAIN == outDsplyConfig->selectVideoOntop)
    {
        tempReg9 |=CSL_VPS_VCOMP_REG9_CFG_MAIN_AUX_N_ONTOP_MASK;
    }

    /* Configure REG 10 */
    /* set timeout */
    if(NULL != timeout)
    {
        tempReg10 = *timeout;
    }

    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->REG6 = tempReg6;
        regOvly->REG9 = tempReg9;

        if(NULL != timeout)
        {
            regOvly->REG10 = tempReg10;
        }
    }
    else
    {
         /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->virRegOffset[VCOMP_CONFIG_OVLY_REG6_IDX];
         *virRegOffset = tempReg6;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->virRegOffset[VCOMP_CONFIG_OVLY_REG9_IDX];
         *virRegOffset = tempReg9;

         if(NULL != timeout)
         {
             virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->virRegOffset[VCOMP_CONFIG_OVLY_REG10_IDX];
             *virRegOffset = tempReg10;
         }
     }
     return (ret);

}

static Int vcompSetVideoMain(VpsHal_Handle handle,
                               const VpsHal_VcompInputPathConfig *mainConfig,
                               const VpsHal_VcompNfHandling *nfHandling,
                               Ptr configOvlyPtr)
{
    Int                    ret = 0;
    CSL_Vps_vcompRegs      *regOvly;
    UInt32                 tempReg0, tempReg1, tempReg2, tempReg7, tempReg11;
    Vcomp_Obj              *vcompObj;
    UInt32                 *virRegOffset;


    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    GT_assert( GT_DEFAULT_MASK, NULL != mainConfig);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    /* Set to 0 */
    tempReg0 = tempReg1 = tempReg2 = tempReg7 = tempReg11 = 0;
    /* Configure R2G 0 regite2 */
    /* enable main video path */
    if(mainConfig->enabled)
    {
        tempReg0 |= CSL_VPS_VCOMP_REG0_CFG_MAIN_ENABLE_MASK;
    }

    /* enable alternate color */
    if(mainConfig->enableFixedData)
    {
        tempReg0 |= CSL_VPS_VCOMP_REG0_CFG_MAIN_FIXED_DATA_SEND_MASK;
    }

    /* set number of lines in a field or frame from the incoming main source */
    tempReg0 |= mainConfig->numLines <<
              CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMLINES_SHIFT;

    /* set number of pixels per line from the incoming main source */
    tempReg0 |= mainConfig->numPix <<
              CSL_VPS_VCOMP_REG0_CFG_MAIN_NATIVE_NUMPIX_PER_LINE_SHIFT;
    vcompObj->mainWidth = mainConfig->numPix;
    vcompObj->mainHeight = mainConfig->numLines;


    /* Configure REG 1 */
    /* Set  number of incoming pixels to discard as part of the Compositor's
     * picture clipping feature */
    tempReg1 |= mainConfig->clipWindow.skipNumPix <<
               CSL_VPS_VCOMP_REG1_CFG_MAIN_SKIP_NUMPIX_SHIFT;
    /* Set number of pixels to use from each incoming line */
    tempReg1 |= mainConfig->clipWindow.useNumPix <<
               CSL_VPS_VCOMP_REG1_CFG_MAIN_USE_NUMPIX_SHIFT;
    /*Set start up condition */
    if(VPSHAL_VCOMP_START_UP_VIDEO_IN_NO_OUT == mainConfig->startUpHandling)
    {
        tempReg1 |= CSL_VPS_VCOMP_REG1_CFG_MAIN_STARTUP_HANDLING_MASK;
    }


    /* Configure REG 2 */
    /* Set   number of incoming lines to discard as part of the Compositor's
     * picture clipping feature */
    tempReg2 |= mainConfig->clipWindow.skipNumLines <<
               CSL_VPS_VCOMP_REG2_CFG_MAIN_SKIP_NUMLINES_SHIFT;
    /*  number of lines to use from each incoming field or frame */
    tempReg2 |= mainConfig->clipWindow.useNumLines <<
               CSL_VPS_VCOMP_REG2_CFG_MAIN_USE_NUMLINES_SHIFT;


    /* Configure REG 7 */
    /* Set Row origin index for the Main source picture */
    tempReg7 |= mainConfig->dsplyPos.yOrigin <<
               CSL_VPS_VCOMP_REG7_CFG_DSPLY_MAIN_Y_ORIGIN_SHIFT;
    /* Set column origin index for the Main source picture */
    tempReg7 |= mainConfig->dsplyPos.xOrigin <<
               CSL_VPS_VCOMP_REG7_CFG_DSPLAY_MAIN_X_ORIGIN_SHIFT;


    /* Configure REG 11 */
    /* Set Alternate Main Y/Luma value for the background display output */
    tempReg11 |= mainConfig->altColor.yLuma <<
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_Y_VAL_SHIFT;
    /* Set Alternate Main Cr/Chroma value for the background display output */
    tempReg11 |= mainConfig->altColor.crChroma <<
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_CR_VAL_SHIFT;
    /* Set Alternate Main Cr/Chroma value for the background display output */
    tempReg11 |= mainConfig->altColor.cbChroma <<
               CSL_VPS_VCOMP_REG11_CFG_DSPLY_ALT_MAIN_CB_VAL_SHIFT;

    if(NULL != nfHandling)
    {
      tempReg11  |= *nfHandling <<
               CSL_VPS_VCOMP_REG11_CFG_NF_HANDLING_SHIFT;
    }
    else
    {
         tempReg11 |= (regOvly->REG11 &
                       CSL_VPS_VCOMP_REG11_CFG_NF_HANDLING_MASK);
    }

    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->REG1 = tempReg1;
        regOvly->REG2 = tempReg2;
        regOvly->REG7 = tempReg7;
        regOvly->REG11 = tempReg11;
        regOvly->REG0 = tempReg0;
    }
    else
    {
         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->mainVirRegOffset[VCOMP_RTCONFIG_OVLY_REG0_IDX];
         *virRegOffset = tempReg0;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->mainVirRegOffset[VCOMP_RTCONFIG_OVLY_REG1_IDX];
         *virRegOffset = tempReg1;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->mainVirRegOffset[VCOMP_RTCONFIG_OVLY_REG2_IDX];
         *virRegOffset = tempReg2;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->mainVirRegOffset[VCOMP_RTCONFIG_OVLY_REG7_IDX];
         *virRegOffset = tempReg7;
     }
     return (ret);
}


static Int vcompSetVideoAux(VpsHal_Handle handle,
                               const VpsHal_VcompInputPathConfig *auxConfig,
                               const VpsHal_VcompDwnStrmSrcFid *dwnStrmSrcFid,
                               const VpsHal_VcompDwnStrmEncFid *dwnStrmEncodFid,
                               Ptr configOvlyPtr)
{
    Int                    ret = 0;
    CSL_Vps_vcompRegs      *regOvly;
    UInt32                 tempReg3, tempReg4, tempReg5, tempReg8, tempReg12;
    Vcomp_Obj              *vcompObj;
    UInt32                 *virRegOffset;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    vcompObj = (Vcomp_Obj *) handle;
    regOvly = vcompObj->regOvly;

    /* Set to 0 */
    tempReg3 = tempReg4 = tempReg5 = tempReg8 = tempReg12 = 0;

    /* Configure REG 3 register */
    /* enable main video path */
    if(TRUE == auxConfig->enabled)
    {
        tempReg3 |= CSL_VPS_VCOMP_REG3_CFG_AUX_ENABLE_MASK;
    }

    /* enable alternate color */
    if(TRUE == auxConfig->enableFixedData)
    {
        tempReg3 |= CSL_VPS_VCOMP_REG3_CFG_AUX_FIXED_DATA_SEND_MASK;
    }

    /* set number of lines in a field or frame from the incoming main source */
    tempReg3 |= auxConfig->numLines <<
              CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMLINES_SHIFT;

    /* set number of pixels per line from the incoming main source */
    tempReg3 |= auxConfig->numPix <<
              CSL_VPS_VCOMP_REG3_CFG_AUX_NATIVE_NUMPIX_PER_LINE_SHIFT;
    vcompObj->auxWidth = auxConfig->numPix;
    vcompObj->auxHeight = auxConfig->numLines;

    /* Configure REG 4 */
    /* Set  number of incoming pixels to discard as part of the Compositor's
     * picture clipping feature */
    tempReg4 |= auxConfig->clipWindow.skipNumPix <<
               CSL_VPS_VCOMP_REG4_CFG_AUX_SKIP_NUMPIX_SHIFT;
    /* Set number of pixels to use from each incoming line */
    tempReg4 |= auxConfig->clipWindow.useNumPix <<
               CSL_VPS_VCOMP_REG4_CFG_AUX_USE_NUMPIX_SHIFT;
    /*Set start up condition */
    if(VPSHAL_VCOMP_START_UP_VIDEO_IN_NO_OUT == auxConfig->startUpHandling)
    {
        tempReg4 |= CSL_VPS_VCOMP_REG4_CFG_AUX_STARTUP_HANDLING_MASK;
    }


    /* Configure REG 5 */
    /* Set   number of incoming lines to discard as part of the Compositor's
     * picture clipping feature */
    tempReg5 |= auxConfig->clipWindow.skipNumLines <<
               CSL_VPS_VCOMP_REG5_CFG_AUX_SKIP_NUMLINES_SHIFT;
    /*  number of lines to use from each incoming field or frame */
    tempReg5 |= auxConfig->clipWindow.useNumLines <<
               CSL_VPS_VCOMP_REG5_CFG_AUX_USE_NUMLINES_SHIFT;


    /* Configure REG 8 */
    /* Set Row origin index for the Main source picture */
    tempReg8 |= auxConfig->dsplyPos.yOrigin <<
               CSL_VPS_VCOMP_REG8_CFG_DSPLY_AUX_Y_ORIGIN_SHIFT;
    /* Set column origin index for the Main source picture */
    tempReg8 |= auxConfig->dsplyPos.xOrigin <<
               CSL_VPS_VCOMP_REG8_CFG_DSPLAY_AUX_X_ORIGIN_SHIFT;

     /*Set src down stream Fid*/
     if(NULL != dwnStrmSrcFid)
     {
         tempReg8 |= *dwnStrmSrcFid <<
               CSL_VPS_VCOMP_REG8_CFG_DWNSTRM_SRC_FID_CTRL_SHIFT;
     }
     else
     {
         tempReg8 |= (regOvly->REG8 &
                       CSL_VPS_VCOMP_REG8_CFG_DWNSTRM_SRC_FID_CTRL_MASK);
     }

    /* Configure REG 12 */
    /* Set Alternate Main Y/Luma value for the background display output */
    tempReg12 |= auxConfig->altColor.yLuma <<
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_Y_VAL_SHIFT;
    /* Set Alternate Main Cr/Chroma value for the background display output */
    tempReg12 |= auxConfig->altColor.crChroma <<
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_CR_VAL_SHIFT;
    /* Set Alternate Main Cr/Chroma value for the background display output */
    tempReg12 |= auxConfig->altColor.cbChroma <<
               CSL_VPS_VCOMP_REG12_CFG_DSPLY_ALT_AUX_CB_VAL_SHIFT;

    /*Set encoder down stream Fid*/
     if(NULL != dwnStrmSrcFid)
     {
         tempReg12 |= *dwnStrmEncodFid <<
               CSL_VPS_VCOMP_REG12_CFG_DWNSTRM_ENC_FID_CTRL_SHIFT;
     }
     else
     {
          tempReg12 |= (regOvly->REG12 &
                       CSL_VPS_VCOMP_REG12_CFG_DWNSTRM_ENC_FID_CTRL_MASK);
     }

    if (NULL == configOvlyPtr)
    {
        /* Write to the actual register */
        regOvly->REG4 = tempReg4;
        regOvly->REG5 = tempReg5;
        regOvly->REG8 = tempReg8;
        regOvly->REG12 = tempReg12;
        regOvly->REG3 = tempReg3;
    }
    else
    {
         /* Write to config register overlay at the correct offset. Since the
         * offset is in words, it can be directly added to the pointer so that
         * (UInt32 *) addition will result in proper offset value */
         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->auxVirRegOffset[VCOMP_RTCONFIG_OVLY_REG0_IDX];
         *virRegOffset = tempReg3;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->auxVirRegOffset[VCOMP_RTCONFIG_OVLY_REG1_IDX];
         *virRegOffset = tempReg4;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->auxVirRegOffset[VCOMP_RTCONFIG_OVLY_REG2_IDX];
         *virRegOffset = tempReg5;

         virRegOffset = (UInt32 *) configOvlyPtr +
                        vcompObj->auxVirRegOffset[VCOMP_RTCONFIG_OVLY_REG7_IDX];
         *virRegOffset = tempReg8;
     }
     return (ret);
}
