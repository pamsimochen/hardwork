/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_sdvenc.c
 *
 * \brief VPS Chroma Up Sampler HAL Source file.
 * This file implements the HAL APIs of the VPS Chroma Up Sampler (420 -> 422).
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/hal/vpshal_sdvenc.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/cslr/cslr_vps_sd_venc.h>
#include <ti/psp/vps/hal/src/vpshalSdVencDefaults.h>

#include <ti/psp/vps/hal/vpshal_onChipEncOps.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* These are not used. But put as a fall back option for bringup. Will
 * be removed after bringup
 */
#define SDVENC_MAX_MODE         (2u)
#define SDVENC_MAX_MODE_NAME    (10u)

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
    SDVENC_CONFIGOVLYIDX_REG0 = 0,
    SDVENC_CONFIGOVLYIDX_REG1,
    SDVENC_CONFIGOVLYIDX_REG2,
    SDVENC_CONFIGOVLYIDX_REG3,
    SDVENC_CONFIGOVLYIDX_REG4,
    SDVENC_CONFIGOVLYIDX_REG5,
    SDVENC_CONFIGOVLYIDX_REG6,
    SDVENC_CONFIGOVLYIDX_REG7,
    SDVENC_CONFIGOVLYIDX_MAX
} SdVenc_ConfigOvlyIdx;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure containing Instance Information for each SDVENC module
 *  parent -            Pointer to the main scalar info structure
 */
typedef struct
{
    UInt32                  standard;
    UInt16                  width;
    UInt16                  height;
    UInt8                   fmt;
    UInt8                   pxlr;
    UInt16                  horzNumClks;
    UInt16                  vertNumClks;
    UInt8                   isSdMode;
    VpsHal_SdVencFrameMode  frameMode;
} SdVenc_ModeInfo;

typedef struct
{
    UInt32                  instId;
    UInt8                   openCnt;
    CSL_VpsSdVencRegsOvly   regOvly;
    VpsHal_SdVencConfig     vencCfg;
    Vps_DcModeInfo           mode;
    /* On-Chip encoder specifics */
    Vps_OnChipEncoderOps    enc[VPSHAL_SDVENC_MAX_NUM_OF_ONCHIP_ENC];
    /**< Operations for all the on-chip encoder that are paired with this
         instance of venc */
    UInt32                  noOfEncs;
    /**< Number of on-chip encoder that this instance of venc supports. */
    VpsHal_SdVencDacConfig  dacConfig;

    VpsHal_SdVencCscCoeff   cscCoeff;

    Int32                   brightness;
    Int32                   contrast;
    Int32                   saturation;
    Int32                   hue;
} SdVenc_Obj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
Int VpsHal_sdVencSetConfig(VpsHal_Handle handle, VpsHal_SdVencConfig *vencCfg);
Void sdVencSetDtvConfig(const vpsHal_SdVencDtvConfig *dtvCfg,
                       CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetDvoConfig(const vpsHal_SdVencDvoConfig *dvoCfg,
                       CSL_VpsSdVencRegsOvly regOvly);
Void sdVencTvDetectConfig(const VpsHal_SdVencTvDetConfig *tvDetCfg,
                          CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetPolarityConfig(const vpsHal_SdVencDtvConfig *dtvCfg,
                             const vpsHal_SdVencDvoConfig *dvoCfg,
                             const VpsHal_SdVencTvDetConfig *tvDetCfg,
                             const CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetSlaveCfg(const vpsHal_SdVencSalveConfig *slvCfg,
                        CSL_VpsSdVencRegsOvly    regOvly);
Void sdVencSetControlConfig(VpsHal_SdVencConfig *vencCfg,
                            CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetEtmgConfig(const vpsHal_SdVencEtmgConfig *eTmgCfg,
                         CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetEtmgConfig(const vpsHal_SdVencEtmgConfig *eTmgCfg,
                         CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetCvbsConfig(const vpsHal_SdVencCvbsConfig *cvbsCfg,
                         CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetCompConfig(const vpsHal_SdVencComponentConfig *comCfg,
                         CSL_VpsSdVencRegsOvly regOvly);
Void sdVencDacConfig(const VpsHal_SdVencDacConfig *dacCfg,
                     CSL_VpsSdVencRegsOvly regOvly);
Void sdVencCgmsConfig(const VpsHal_SdVencCgmsConfig *cgmsCfg,
                      CSL_VpsSdVencRegsOvly regOvly);
Void sdVencWssConfig (const VpsHal_SdVencWssConfig *wssCfg,
                      CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetLpfCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSet2xSamCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSet2xOverSamCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly);
Void sdVencCcConfig(const VpsHal_SdVencClosedCapConfig *ccCfg,
                    CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetLpfCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly);
Void sdVencSetCvbsCscCoeffs(SdVenc_Obj              *sdVencObj,
                            VpsHal_SdVencCscCoeff   *coeffPtr);
Void sdVencSetCompCscCoeffs(SdVenc_Obj              *sdVencObj,
                            VpsHal_SdVencCscCoeff   *coeffPtr);
Void sdVencGetDtvConfig(vpsHal_SdVencDtvConfig *dtvCfg,
                       CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetDvoConfig(vpsHal_SdVencDvoConfig *dvoCfg,
                       CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetTvDetectConfig(VpsHal_SdVencTvDetConfig *tvDetCfg,
                          CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetPolarityConfig(vpsHal_SdVencDtvConfig *dtvCfg,
                             vpsHal_SdVencDvoConfig *dvoCfg,
                             VpsHal_SdVencTvDetConfig *tvDetCfg,
                             CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetSlaveCfg(vpsHal_SdVencSalveConfig *slvCfg,
                        CSL_VpsSdVencRegsOvly    regOvly);
Void sdVencGetControlConfig(VpsHal_SdVencConfig *vencCfg,
                            CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetEtmgConfig(vpsHal_SdVencEtmgConfig *eTmgCfg,
                         CSL_VpsSdVencRegsOvly regOvly);

Void sdVencGetCvbsConfig(vpsHal_SdVencCvbsConfig *cvbsCfg,
                         CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetCompConfig(vpsHal_SdVencComponentConfig *comCfg,
                         CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetDacConfig(VpsHal_SdVencDacConfig *dacCfg,
                     CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetCgmsConfig(VpsHal_SdVencCgmsConfig *cgmsCfg,
                      CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetWssConfig (VpsHal_SdVencWssConfig *wssCfg,
                      CSL_VpsSdVencRegsOvly regOvly);
Void sdVencGetCcConfig(VpsHal_SdVencClosedCapConfig *ccCfg,
                    CSL_VpsSdVencRegsOvly regOvly);
static Int32 VpsHal_sdVencSetControl(SdVenc_Obj *sdVencObj, Vps_DcVencControl *ctrl);
static Int32 VpsHal_sdVencGetControl(SdVenc_Obj *sdVencObj, Vps_DcVencControl *ctrl);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


static SdVenc_Obj SdVencObj[VPSHAL_SDVENC_MAX_INST];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_sdvencInit
 *  \brief Initializes SDVENC objects, gets the register overlay offsets for
 *  SDVENC registers.
 *  This function should be called before calling any of SDVENC HAL API's.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              Not used currently. Meant for future purpose.
 */
Int VpsHal_sdvencInit(UInt32 numInstances,
                     const VpsHal_SdVencInstParams *initParams,
                     Ptr arg)
{
    Int                     instCnt;
    Int                     ret = 0;
    UInt32                  encCnt = 0;
    SdVenc_Obj             *instInfo = NULL;
    Vps_DcOutputInfo        dcOutputInfo;

    /* Check for errors */
    GT_assert( GT_DEFAULT_MASK, numInstances <= VPSHAL_SDVENC_MAX_INST);
    GT_assert( GT_DEFAULT_MASK, initParams != NULL);

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < numInstances; instCnt ++)
    {
        GT_assert( GT_DEFAULT_MASK, initParams[instCnt].instId < VPSHAL_SDVENC_MAX_INST);

        /* Initialize Instance Specific Parameters */
        instInfo = &(SdVencObj[instCnt]);
        instInfo->openCnt = 0u;
        instInfo->regOvly =
                (CSL_VpsSdVencRegsOvly ) initParams[instCnt].baseAddress;
        instInfo->instId = initParams[instCnt].instId;

        /* Paired encoder details */
        instInfo->noOfEncs = initParams[instCnt].pairedEncCnt;

        for (encCnt = 0x0; encCnt < initParams[instCnt].pairedEncCnt; encCnt++)
        {
            instInfo->enc[encCnt].encId = initParams[instCnt].encIds[encCnt];
            instInfo->enc[encCnt].getModeEnc = NULL;
            instInfo->enc[encCnt].setModeEnc = NULL;
            instInfo->enc[encCnt].startEnc = NULL;
            instInfo->enc[encCnt].stopEnc = NULL;
            instInfo->enc[encCnt].controlEnc = NULL;
            instInfo->enc[encCnt].handle = NULL;
        }

        VpsUtils_memset(&instInfo->dacConfig, 0, sizeof(instInfo->dacConfig));
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        VpsHal_sdvencSetOutput((VpsHal_Handle)instInfo, &dcOutputInfo);

        instInfo->brightness = 0;
        instInfo->contrast = 128;
        instInfo->saturation = 128;
        instInfo->hue = 31;
    }

    return (ret);
}

/**
 *  VpsHal_sdvencDeInit
 *  \brief Currently this function does not do anything.
 *
 *  \param arg       Not used currently. Meant for future purpose.
 */
Int VpsHal_sdvencDeInit(Ptr arg)
{
    return (0);
}

/**
 *  VpsHal_sdvencOpen
 *  \brief This function should be called prior to calling any of the SDVENC HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param sdvencInst       Requested SDVENC instance
 */
VpsHal_Handle VpsHal_sdvencOpen(UInt32 sdvencInst)
{
    Int                     cnt;
    UInt32                  cookie;
    VpsHal_Handle           handle = NULL;
    SdVenc_Obj             *instInfo = NULL;

    for (cnt = 0; cnt < VPSHAL_SDVENC_MAX_INST; cnt++)
    {
        instInfo = &(SdVencObj[cnt]);
        /* Return the matching instance handle */
        if (sdvencInst == instInfo->instId)
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
 *  VpsHal_sdvencClose
 *  \brief This functions closes the sdvenc handle and after call to this
 *  function, sdvenc handle is not valid. It also decrements the reference
 *  counter.
 *
 *  \param handle       Requested SDVENC instance
 */
Int VpsHal_sdvencClose(VpsHal_Handle handle)
{
    Int                 ret = -1;
    UInt32              cookie;
    SdVenc_Obj          *instInfo = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, (NULL != handle));

    instInfo = (SdVenc_Obj *) handle;

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
 *  VpsHal_sdvencStartVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencStartVenc(VpsHal_Handle handle)
{
    SdVenc_Obj              *sdVencObj = NULL;
    CSL_VpsSdVencRegsOvly    regOvly = NULL;
    Int32                   retVal = VPS_SOK;
    UInt32                  encCnt;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    sdVencObj = (SdVenc_Obj *)handle;
    regOvly = sdVencObj->regOvly;

    regOvly->VMOD |= CSL_VPS_SD_VENC_VMOD_VIEN_MASK;

    for(encCnt = 0x0; encCnt < sdVencObj->noOfEncs; encCnt++)
    {
        if (sdVencObj->enc[encCnt].startEnc != NULL)
        {
            retVal = sdVencObj->enc[encCnt].startEnc (
                            sdVencObj->enc[encCnt].handle,
                            NULL);
            if (VPS_SOK != retVal)
            {
                /* Let the apps know the reason */
                break;
            }
        }
    }

    return (retVal);
}

/**
 *  VpsHal_sdvencStopVenc
 *  \brief This functions to set the mode in the SdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencStopVenc(VpsHal_Handle handle)
{
    SdVenc_Obj              *sdVencObj = NULL;
    CSL_VpsSdVencRegsOvly    regOvly = NULL;
    Int32                   retVal = VPS_SOK;
    UInt32                  encCnt;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    sdVencObj = (SdVenc_Obj *)handle;
    regOvly = sdVencObj->regOvly;

    regOvly->VMOD &= (~CSL_VPS_SD_VENC_VMOD_VIEN_MASK);

    for(encCnt = 0x0; encCnt < sdVencObj->noOfEncs; encCnt++)
    {
        if (sdVencObj->enc[encCnt].stopEnc != NULL)
        {
            retVal = sdVencObj->enc[encCnt].stopEnc (
                            sdVencObj->enc[encCnt].handle,
                            NULL);
            if (VPS_SOK != retVal)
            {
                /* Let the apps know the reason */
                break;
            }
        }
    }

    return (retVal);
}

/**
 *  VpsHal_sdvencResetVenc
 *  \brief This functions to reset the mode in the sdVenc.
 *
 *  \param handle         Requested SDVENC instance
 */
Int VpsHal_sdvencResetVenc(VpsHal_Handle handle)
{
    return (VPS_SOK);
}



/**
 * VpsHal_sdVencSetMode
 * \brief This function is used to set the mode on the SDVENC
 *
 * \param handle          Requested SDVENC instance
 *
 * \param mode            Mode information
 */
Int32 VpsHal_sdVencSetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode)
{

    SdVenc_Obj              *sdVencObj = NULL;
    Int32                   retVal = VPS_SOK;
    VpsHal_SdVencConfig     *vencCfg = NULL;
    UInt32                  encCnt;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != mode));

    sdVencObj = (SdVenc_Obj *)handle;
    /* TODO Pixel clock programming is still left */
    /* mode->pixelClock */
    /* Copy first the default parameters */
    /* Set the default parameters according to mode selected */
    sdVencObj->vencCfg.sdVencMode = mode->mInfo.standard;
    vencCfg = &sdVencObj->vencCfg;
    switch (vencCfg->sdVencMode)
    {
        case FVID2_STD_NTSC:
            /* copy the standard parameters for the 1080I60 mode */
            VpsUtils_memcpy(vencCfg,
                        &sdVencNtscCfg,
                        sizeof(VpsHal_SdVencConfig));
            retVal = VPS_SOK;
            break;
        case FVID2_STD_PAL:
            VpsUtils_memcpy(vencCfg,
                        &sdVencPalCfg,
                        sizeof(VpsHal_SdVencConfig));
            break;
        default:
            retVal = VPS_EFAIL;
    }
    if (FVID2_SOK == retVal )
    {
        retVal = VpsHal_sdVencSetConfig(handle, &sdVencObj->vencCfg);
    }
    if (VPS_SOK == retVal)
    {
        switch (vencCfg->sdVencMode)
        {
            case FVID2_STD_NTSC:
                /* copy the standard parameters for the 1080I60 mode */
                VpsUtils_memcpy(&sdVencObj->mode,
                                &standardModes[0],
                                sizeof(Vps_DcModeInfo));
                retVal = VPS_SOK;
                break;
            case FVID2_STD_PAL:
                VpsUtils_memcpy(&sdVencObj->mode,
                                &standardModes[1],
                                sizeof(Vps_DcModeInfo));
                break;
            default:
                retVal = VPS_EFAIL;
        }
    }

    /* For all the paired on-chip encoders - update the new mode */
    if (VPS_SOK == retVal)
    {
        for(encCnt = 0x0; encCnt < sdVencObj->noOfEncs; encCnt++)
        {
            if (sdVencObj->enc[encCnt].setModeEnc != NULL)
            {
                retVal = sdVencObj->enc[encCnt].setModeEnc (
                                sdVencObj->enc[encCnt].handle,
                                mode);
            }
            if (VPS_SOK != retVal)
            {
                /* Let the apps know the reason */
                break;
            }
        }
    }

    return retVal;
}

/**
 *  VpsHal_sdVencSetOutput
 *  \brief This function is used to configure ouptut type for SD Venc
 *
 *  \param handle         Requested SDVENC instance
 *
 *  \param output         Ouput format
 */
Int32 VpsHal_sdvencSetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output)
{
    Int32                            retVal          = VPS_SOK;
    SdVenc_Obj                      *sdVencObj      = NULL;
    VpsHal_SdVencConfig             *vencCfg        = NULL;
    VpsHal_SdVencDacConfig          *dacCfg         = NULL;
    CSL_VpsSdVencRegsOvly            regOvly = NULL;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != output));

    sdVencObj = (SdVenc_Obj *)handle;
    vencCfg = &sdVencObj->vencCfg;
    dacCfg = &vencCfg->dacConfig;
    regOvly = sdVencObj->regOvly;

    switch (output->aFmt)
    {
        case VPS_DC_A_OUTPUT_COMPOSITE:
            /* Turn ON only one DAC as its a composite output */
            dacCfg->dac0    = VPSHAL_SDVENC_DAC_SEL_CVBS;
            dacCfg->enableDac0 = TRUE;
            dacCfg->enableDac1 = FALSE;
            dacCfg->enableDac2 = FALSE;
            dacCfg->enableDac3 = FALSE;
            break;
        case VPS_DC_A_OUTPUT_SVIDEO:
            /* Turn ON two DACs as its a S-video output */
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
            dacCfg->dac0    = VPSHAL_SDVENC_DAC_SEL_SVIDEO_Y;
            dacCfg->dac1    = VPSHAL_SDVENC_DAC_SEL_SVIDEO_C;
            dacCfg->enableDac0 = TRUE;
            dacCfg->enableDac1 = TRUE;
            dacCfg->enableDac2 = FALSE;
            dacCfg->enableDac3 = FALSE;
#endif /* TI_814X_BUILD || TI_8107_BUILD */

#ifdef TI_816X_BUILD
            dacCfg->dac1    = VPSHAL_SDVENC_DAC_SEL_SVIDEO_Y;
            dacCfg->dac2    = VPSHAL_SDVENC_DAC_SEL_SVIDEO_C;
            dacCfg->enableDac0 = FALSE;
            dacCfg->enableDac1 = TRUE;
            dacCfg->enableDac2 = TRUE;
            dacCfg->enableDac3 = FALSE;
#endif /* TI_816X_BUILD */

            break;
        case VPS_DC_A_OUTPUT_COMPONENT:

            /* Turn ON all three DACs as its a component output */
            /* Turn ON two DACs as its a S-video output */
            dacCfg->dac0    = VPSHAL_SDVENC_DAC_SEL_COM_YG;
            dacCfg->dac1    = VPSHAL_SDVENC_DAC_SEL_COM_UB;
            dacCfg->dac1    = VPSHAL_SDVENC_DAC_SEL_COM_VR;
            dacCfg->enableDac0 = TRUE;
            dacCfg->enableDac1 = TRUE;
            dacCfg->enableDac2 = TRUE;
            dacCfg->enableDac3 = FALSE;

            break;
        default :
            retVal = VPS_EINVALID_PARAMS;
    }
    if (VPS_SOK == retVal)
    {
        sdVencDacConfig(dacCfg, regOvly);
    }
    if (VPS_SOK == retVal)
    {
        VpsUtils_memcpy(&sdVencObj->dacConfig,
                        dacCfg,
                        sizeof(sdVencObj->dacConfig));
    }
    return retVal;
}

/**
 *  VpsHal_sdVencSetOutput
 *  \brief This function is used to configure ouptut type for SD Venc
 *
 *  \param handle         Requested SDVENC instance
 *
 *  \param output         Ouput format
 */
Void VpsHal_sdvencGetOutput(VpsHal_Handle handle, Vps_DcOutputInfo *output)
{
    SdVenc_Obj                      *sdVencObj      = NULL;
    VpsHal_SdVencDacConfig          *dacCfg         = NULL;
    CSL_VpsSdVencRegsOvly            regOvly = NULL;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != output));

    sdVencObj = (SdVenc_Obj *)handle;
    dacCfg = &sdVencObj->dacConfig;
    regOvly = sdVencObj->regOvly;

    sdVencGetDacConfig(dacCfg, regOvly);

    if ((VPSHAL_SDVENC_DAC_SEL_CVBS == dacCfg->dac0) &&
        (TRUE == dacCfg->enableDac0))
    {
        output->aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
    }
#ifdef TI_816X_BUILD
    else if ((VPSHAL_SDVENC_DAC_SEL_SVIDEO_Y == dacCfg->dac1) &&
             (TRUE == dacCfg->enableDac1) &&
             (TRUE == dacCfg->enableDac2))
    {
        output->aFmt = VPS_DC_A_OUTPUT_SVIDEO;
    }
#endif

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    else if ((VPSHAL_SDVENC_DAC_SEL_SVIDEO_Y == dacCfg->dac0) &&
             (TRUE == dacCfg->enableDac0) &&
             (TRUE == dacCfg->enableDac1))
    {
        output->aFmt = VPS_DC_A_OUTPUT_SVIDEO;
    }
#endif

    else if (VPSHAL_SDVENC_DAC_SEL_COM_YG == dacCfg->dac0)
    {
        output->aFmt = VPS_DC_A_OUTPUT_COMPONENT;
    }
}

/**
 *  VpsHal_sdVencGetMode
 *  \brief This functions to set the mode in the SDVENC.
 *
 *  \param handle         Requested SDVENC instance
 */
Int32 VpsHal_sdVencGetMode(VpsHal_Handle handle, Vps_DcModeInfo *mode)
{
    SdVenc_Obj              *sdVencObj = NULL;
    UInt32                  vencId;
    UInt32                  encCnt;
    Int32                   retVal  =   VPS_SOK;
    Vps_DcModeInfo          encMode;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != mode));

    sdVencObj = (SdVenc_Obj *)handle;
    vencId = mode->vencId;
    VpsUtils_memcpy(mode,
                    &sdVencObj->mode,
                    sizeof(Vps_DcModeInfo));

    if (VPS_SOK == retVal)
    {
        for(encCnt = 0x0; encCnt < sdVencObj->noOfEncs; encCnt++)
        {
            if (sdVencObj->enc[encCnt].getModeEnc != NULL)
            {
                retVal = sdVencObj->enc[encCnt].getModeEnc (
                                sdVencObj->enc[encCnt].handle,
                                &encMode);
                if (VPS_SOK != retVal)
                {
                    /* Let the apps know the reason */
                    break;
                }
            }
        }
    }

    mode->vencId = vencId;
    return (VPS_SOK);
}

/**
 * VPsHal_sdVencConfig
 * \brief This function allows the full configuration of the
 * SD Venc
 * \brief handle            Requested SDVENC instance
 * \brief vencCfg           Venc configuration
 */
Int VpsHal_sdVencSetConfig(VpsHal_Handle handle, VpsHal_SdVencConfig *vencCfg)
{
    const vpsHal_SdVencDtvConfig *dtvCfg;
    const vpsHal_SdVencDvoConfig *dvoCfg;
    const VpsHal_SdVencTvDetConfig     *tvDetCfg;
    const vpsHal_SdVencSalveConfig *slvCfg;
    const vpsHal_SdVencEtmgConfig *encCfg;
    const vpsHal_SdVencCvbsConfig *cvbsCfg;
    const vpsHal_SdVencComponentConfig *comCfg;
    const VpsHal_SdVencClosedCapConfig *ccCfg;
    const VpsHal_SdVencWssConfig *wssCfg;
    const VpsHal_SdVencCgmsConfig *cgmsCfg;
    SdVenc_Obj              *sdVencObj = NULL;
    CSL_VpsSdVencRegsOvly    regOvly = NULL;
    VpsHal_SdVencCscCoeff   *coeffPtr = NULL;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    sdVencObj = (SdVenc_Obj *)handle;

    dtvCfg      = &vencCfg->dtvConfig;
    dvoCfg      = &vencCfg->dvoConfig;
    tvDetCfg    = &vencCfg->tvDetectConfig;
    slvCfg      = &vencCfg->slaveConfig;
    encCfg      = &vencCfg->encoderTiming;
    cvbsCfg     = &vencCfg->cvbsConfig;
    comCfg      = &vencCfg->componentConfig;
    ccCfg       = &vencCfg->ccConfig;
    wssCfg      = &vencCfg->wssConfig;
    cgmsCfg     = &vencCfg->cgmsConfig;


    regOvly = sdVencObj->regOvly;

    regOvly->VMOD = 0;
    if (FVID2_SF_INTERLACED == vencCfg->scanFormat)
    {
        regOvly->VMOD |= CSL_VPS_SD_VENC_VMOD_ITLC_MASK;
    }
    else
    {
        regOvly->VMOD &= ~CSL_VPS_SD_VENC_VMOD_ITLC_MASK;
    }
    if (VPSHAL_SDVENC_UNEQUAL_LINES == vencCfg->FieldLineSym)
    {
        regOvly->VMOD |= CSL_VPS_SD_VENC_VMOD_UEL_MASK;
    }
    else
    {
        regOvly->VMOD &= ~CSL_VPS_SD_VENC_VMOD_UEL_MASK;
    }
    regOvly->VMOD &= ~CSL_VPS_SD_VENC_VMOD_CBAR_MASK;
    if (TRUE == vencCfg->inputDataInv)
    {
        regOvly->VMOD |= CSL_VPS_SD_VENC_VMOD_DIIV_MASK;
    }
    else
    {
        regOvly->VMOD &= ~CSL_VPS_SD_VENC_VMOD_DIIV_MASK;
    }
    if (TRUE == vencCfg->slave)
    {
        regOvly->SLAVE = 0;
        regOvly->SLAVE |= CSL_VPS_SD_VENC_SLAVE_SLV_MASK;
        sdVencSetSlaveCfg(slvCfg, regOvly);

    }
    else
    {
        regOvly->SLAVE &= ~CSL_VPS_SD_VENC_SLAVE_SLV_MASK;
    }
    regOvly->SIZE = ((vencCfg->totalPixels << CSL_VPS_SD_VENC_SIZE_HITV_SHIFT) &
                        CSL_VPS_SD_VENC_SIZE_HITV_MASK) |
                    ((vencCfg->totalLines << CSL_VPS_SD_VENC_SIZE_VITV_SHIFT) &
                        CSL_VPS_SD_VENC_SIZE_VITV_MASK);
    sdVencSetPolarityConfig(dtvCfg, dvoCfg, tvDetCfg, regOvly);
    sdVencSetDtvConfig(dtvCfg, regOvly);
    sdVencSetDvoConfig(dvoCfg, regOvly);
    sdVencTvDetectConfig(tvDetCfg, regOvly);
    regOvly->IRQ0 = ((vencCfg->irqOutputStartPixel <<
                        CSL_VPS_SD_VENC_IRQ0_IRQ_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_IRQ0_IRQ_H_STA_MASK) |
                    ((vencCfg->irqOutputStartLine <<
                        CSL_VPS_SD_VENC_IRQ0_IRQ_V_STA_SHIFT) &
                        CSL_VPS_SD_VENC_IRQ0_IRQ_V_STA_MASK);
    sdVencSetControlConfig(vencCfg, regOvly);
    sdVencSetEtmgConfig(encCfg, regOvly);
    sdVencSetCvbsConfig(cvbsCfg, regOvly);
    sdVencSetCompConfig(comCfg, regOvly);
    sdVencSetLpfCoeffs(vencCfg, regOvly);
    sdVencSet2xSamCoeffs(vencCfg, regOvly);
    sdVencSet2xOverSamCoeffs(vencCfg, regOvly);
    sdVencCcConfig(ccCfg, regOvly);
    sdVencWssConfig(wssCfg, regOvly);
    sdVencCgmsConfig(cgmsCfg, regOvly);
    regOvly->SCCTL0 = ((vencCfg->subCarrierIniPhase <<
                            CSL_VPS_SD_VENC_SCCTL0_SCSD_SHIFT) &
                            CSL_VPS_SD_VENC_SCCTL0_SCSD_MASK) |
                       ((vencCfg->subCarrierFreqPar0 <<
                            CSL_VPS_SD_VENC_SCCTL0_SCP0_SHIFT) &
                            CSL_VPS_SD_VENC_SCCTL0_SCP0_MASK);
    regOvly->SCCTL1 = ((vencCfg->subCarrierFreqPar1 <<
                            CSL_VPS_SD_VENC_SCCTL1_SCP1_SHIFT) &
                            CSL_VPS_SD_VENC_SCCTL1_SCP1_MASK) |
                       ((vencCfg->subCarrierFreqPar2 <<
                            CSL_VPS_SD_VENC_SCCTL1_SCP2_SHIFT) &
                            CSL_VPS_SD_VENC_SCCTL1_SCP2_MASK);

    if (FVID2_STD_NTSC == vencCfg->sdVencMode)
    {
        sdVencObj->cscCoeff = defaultCvbsCscCoeffs_NTSC;
    }
    else
    {
        sdVencObj->cscCoeff = defaultCvbsCscCoeffs_PAL;
    }

    coeffPtr = &sdVencObj->cscCoeff;
    sdVencSetCvbsCscCoeffs(sdVencObj, coeffPtr);

    if (FVID2_STD_NTSC == vencCfg->sdVencMode)
    {
        coeffPtr = &defaultComponentCscCoeffs_NTSC;
    }
    else
    {
        coeffPtr = &defaultComponentCscCoeffs_PAL;
    }
    sdVencSetCompCscCoeffs(sdVencObj, coeffPtr);
    return (VPS_SOK);
}

/**
 * \brief Initializes venc with capabilities of paired on-chip encoders
 *
 * \param handle    Handle to SD Venc HAL
 * \param encOps    Capabilities of the encoder
 */
Int32 VpsHal_sdvencInitEncOps(VpsHal_Handle handle,
                                    Vps_OnChipEncoderOps *encOps)
{
    Int32       rtnValue    =   VPS_EBADARGS;
    UInt32      noOfEncs    =   0x0;
    SdVenc_Obj  *sdVencObj  =   NULL;
    UInt32  cookie;
    GT_assert( VpsHalTrace, (NULL != encOps));
    GT_assert( VpsHalTrace, (NULL != handle));

    /*
     * 1. Validate the arguments, including venc ID.
     * 2. Validate the venc status - if streaming, return error
     * 3. Copy the ops supplied - under interrupt protection
     */
    while (TRUE)
    {
        if (encOps == NULL)
        {
            break;
        }
        sdVencObj = (SdVenc_Obj *)handle;
        for (   noOfEncs = 0x0;
                noOfEncs < VPSHAL_SDVENC_MAX_NUM_OF_ONCHIP_ENC;
                noOfEncs++)
        {
            if (sdVencObj->enc[noOfEncs].encId == encOps[noOfEncs].encId)
            {
                /* Exclusive access */
                cookie = Hwi_disable();

                /* Found the match, copy the ops */
                sdVencObj->enc[noOfEncs].getModeEnc =
                                                    encOps[noOfEncs].getModeEnc;
                sdVencObj->enc[noOfEncs].setModeEnc =
                                                    encOps[noOfEncs].setModeEnc;
                sdVencObj->enc[noOfEncs].startEnc   = encOps[noOfEncs].startEnc;
                sdVencObj->enc[noOfEncs].stopEnc    = encOps[noOfEncs].stopEnc;
                sdVencObj->enc[noOfEncs].controlEnc =
                                                    encOps[noOfEncs].controlEnc;
                sdVencObj->enc[noOfEncs].handle     = encOps[noOfEncs].handle;

                Hwi_restore(cookie);

                rtnValue    =   VPS_SOK;
                break;
            } /* Matching encoder IDs */
            else
            {
                rtnValue = VPS_EBADARGS;
                break;
            }
        } /* All supported encoder on this venc */
        break;
    }

    return (rtnValue);
}

/**
 *  \brief This functions controls the VENC and other paired encoders. For now,
 *         there are no VENC specific control operations that could be performed.
 *
 *         If paired encoder (such as RF Modulator) for this VENC exists,
 *         the paired encoder's control function would be called.
 *
 *  \param VpsHal_Handle    Handle to the venc
 *  \param cmd              Control command
 *  \param cmdArgs          Arguments associated with the command
 *  \param cmdStatusArgs    Result after execution of the command
 */
Int32 VpsHal_sdvencIoctl(VpsHal_Handle handle,
                            UInt32          cmd,
                            Ptr             cmdArgs,
                            Ptr             cmdStatusArgs)
{
    Int32       retVal      =   VPS_SOK;
    SdVenc_Obj  *sdVencObj  =   NULL;
    UInt32      encCnt;

    GT_assert( VpsHalTrace, (NULL != handle));

    sdVencObj = (SdVenc_Obj *)handle;

    switch (cmd)
    {
        case IOCTL_VPS_DCTRL_SET_VENC_CONTROL:
        {
            Vps_DcVencControl *ctrl = (Vps_DcVencControl *)cmdArgs;

            retVal = VpsHal_sdVencSetControl(sdVencObj, ctrl);

            break;
        }

        case IOCTL_VPS_DCTRL_GET_VENC_CONTROL:
        {
            Vps_DcVencControl *ctrl = (Vps_DcVencControl *)cmdArgs;

            retVal = VpsHal_sdVencGetControl(sdVencObj, ctrl);

            break;
        }
        default:
        {
            for(encCnt = 0x0; encCnt < sdVencObj->noOfEncs; encCnt++)
            {
                if (sdVencObj->enc[encCnt].controlEnc != NULL)
                {
                    retVal = sdVencObj->enc[encCnt].controlEnc (
                                    sdVencObj->enc[encCnt].handle,
                                    cmd,
                                    cmdArgs,
                                    cmdStatusArgs);
                    if (VPS_SOK != retVal)
                    {
                        /* Let the apps know the reason */
                        break;
                    }
                }
            }
        }
    }

    return (retVal);
}

Void sdVencSetCompCscCoeffs(SdVenc_Obj              *sdVencObj,
                            VpsHal_SdVencCscCoeff   *coeffPtr)
{
    CSL_VpsSdVencRegsOvly    regOvly = NULL;

    GT_assert( VpsHalTrace, (NULL != sdVencObj));

    regOvly = sdVencObj->regOvly;

    GT_assert( VpsHalTrace, (NULL != regOvly));
    GT_assert( VpsHalTrace, (NULL != coeffPtr));

    regOvly->MCSC0 = ((coeffPtr->mulCoeff[0][0] <<
                        CSL_VPS_SD_VENC_MCSC0_MCSCA0_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC0_MCSCA0_MASK) |
                      ((coeffPtr->mulCoeff[0][1] <<
                        CSL_VPS_SD_VENC_MCSC0_MCSCB0_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC0_MCSCB0_MASK);
    regOvly->MCSC3 = ((coeffPtr->mulCoeff[1][0] <<
                        CSL_VPS_SD_VENC_MCSC3_MCSCA1_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC3_MCSCA1_MASK) |
                      ((coeffPtr->mulCoeff[1][1] <<
                        CSL_VPS_SD_VENC_MCSC3_MCSCB1_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC3_MCSCB1_MASK);
    regOvly->MCSC6 = ((coeffPtr->mulCoeff[2][0] <<
                        CSL_VPS_SD_VENC_MCSC6_MCSCA2_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC6_MCSCA2_MASK) |
                      ((coeffPtr->mulCoeff[2][1] <<
                        CSL_VPS_SD_VENC_MCSC6_MCSCB2_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC6_MCSCB2_MASK);

    regOvly->MCSC1 = ((coeffPtr->mulCoeff[0][2] <<
                        CSL_VPS_SD_VENC_MCSC1_MCSCC0_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC1_MCSCC0_MASK) |
                      ((coeffPtr->inOff[0] <<
                        CSL_VPS_SD_VENC_MCSC1_MCSCD0_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC1_MCSCD0_MASK);

    regOvly->MCSC4 = ((coeffPtr->mulCoeff[1][2] <<
                        CSL_VPS_SD_VENC_MCSC4_MCSCC1_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC4_MCSCC1_MASK) |
                    ((coeffPtr->inOff[0] <<
                        CSL_VPS_SD_VENC_MCSC4_MCSCD1_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC4_MCSCD1_MASK);
    regOvly->MCSC7 = ((coeffPtr->mulCoeff[1][2] <<
                        CSL_VPS_SD_VENC_MCSC7_MCSCC2_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC7_MCSCC2_MASK) |
                    ((coeffPtr->inOff[0] <<
                        CSL_VPS_SD_VENC_MCSC7_MCSCD2_SHIFT) &
                        CSL_VPS_SD_VENC_MCSC7_MCSCD2_MASK);
    regOvly->MCSC2 = ((coeffPtr->outOff[0] <<
                            CSL_VPS_SD_VENC_MCSC2_MCSCE0_SHIFT) &
                            CSL_VPS_SD_VENC_MCSC2_MCSCE0_MASK);
    regOvly->MCSC5 = ((coeffPtr->outOff[1] <<
                            CSL_VPS_SD_VENC_MCSC5_MCSCE1_SHIFT) &
                            CSL_VPS_SD_VENC_MCSC5_MCSCE1_MASK);
    regOvly->MCSC8 = ((coeffPtr->outOff[2] <<
                            CSL_VPS_SD_VENC_MCSC8_MCSCE2_SHIFT) &
                            CSL_VPS_SD_VENC_MCSC8_MCSCE2_MASK);
}

Void sdVencSetCvbsCscCoeffs(SdVenc_Obj              *sdVencObj,
                            VpsHal_SdVencCscCoeff   *coeffPtr)
{
    CSL_VpsSdVencRegsOvly    regOvly = NULL;

    GT_assert( VpsHalTrace, (NULL != sdVencObj));

    regOvly = sdVencObj->regOvly;

    GT_assert( VpsHalTrace, (NULL != regOvly));
    GT_assert( VpsHalTrace, (NULL != coeffPtr));

    /*coeffPtr = vencCfg->cvbsCoeffs;
    if (NULL == coeffPtr)
    {
        if (FVID2_STD_NTSC == vencCfg->sdVencMode)
        {
            coeffPtr = &defaultCvbsCscCoeffs_NTSC;
        }
        else
        {
            coeffPtr = &defaultCvbsCscCoeffs_PAL;
        }
    }*/

    regOvly->CCSC0 = ((coeffPtr->mulCoeff[0][0] <<
                        CSL_VPS_SD_VENC_CCSC0_CCSCA0_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC0_CCSCA0_MASK) |
                      ((coeffPtr->mulCoeff[0][1] <<
                        CSL_VPS_SD_VENC_CCSC0_CCSCB0_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC0_CCSCB0_MASK);
    regOvly->CCSC3 = ((coeffPtr->mulCoeff[1][0] <<
                        CSL_VPS_SD_VENC_CCSC3_CCSCA1_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC3_CCSCA1_MASK) |
                      ((coeffPtr->mulCoeff[1][1] <<
                        CSL_VPS_SD_VENC_CCSC3_CCSCB1_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC3_CCSCB1_MASK);
    regOvly->CCSC6 = ((coeffPtr->mulCoeff[2][0] <<
                        CSL_VPS_SD_VENC_CCSC6_CCSCA2_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC6_CCSCA2_MASK) |
                      ((coeffPtr->mulCoeff[2][1] <<
                        CSL_VPS_SD_VENC_CCSC6_CCSCB2_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC6_CCSCB2_MASK);

    regOvly->CCSC1 = ((coeffPtr->mulCoeff[0][2] <<
                        CSL_VPS_SD_VENC_CCSC1_CCSCC0_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC1_CCSCC0_MASK) |
                      ((coeffPtr->inOff[0] <<
                        CSL_VPS_SD_VENC_CCSC1_CCSCD0_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC1_CCSCD0_MASK);

    regOvly->CCSC4 = ((coeffPtr->mulCoeff[1][2] <<
                        CSL_VPS_SD_VENC_CCSC4_CCSCC1_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC4_CCSCC1_MASK) |
                    ((coeffPtr->inOff[0] <<
                        CSL_VPS_SD_VENC_CCSC4_CCSCD1_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC4_CCSCD1_MASK);
    regOvly->CCSC7 = ((coeffPtr->mulCoeff[2][2] <<
                        CSL_VPS_SD_VENC_CCSC7_CCSCC2_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC7_CCSCC2_MASK) |
                    ((coeffPtr->inOff[0] <<
                        CSL_VPS_SD_VENC_CCSC7_CCSCD2_SHIFT) &
                        CSL_VPS_SD_VENC_CCSC7_CCSCD2_MASK);
    regOvly->CCSC2 = ((coeffPtr->outOff[0] <<
                            CSL_VPS_SD_VENC_CCSC2_CCSCE0_SHIFT) &
                            CSL_VPS_SD_VENC_CCSC2_CCSCE0_MASK);
    regOvly->CCSC5 = ((coeffPtr->outOff[1] <<
                            CSL_VPS_SD_VENC_CCSC5_CCSCE1_SHIFT) &
                            CSL_VPS_SD_VENC_CCSC5_CCSCE1_MASK);
    regOvly->CCSC8 = ((coeffPtr->outOff[2] <<
                            CSL_VPS_SD_VENC_CCSC8_CCSCE2_SHIFT) &
                            CSL_VPS_SD_VENC_CCSC8_CCSCE2_MASK);
}

Void sdVencDacConfig(const VpsHal_SdVencDacConfig *dacCfg,
                     CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->DACSEL = 0;
    regOvly->DACSEL |= ((dacCfg->dac0 <<
                            CSL_VPS_SD_VENC_DACSEL_DA0S_SHIFT) &
                            CSL_VPS_SD_VENC_DACSEL_DA0S_MASK) |
                       ((dacCfg->dac1 <<
                            CSL_VPS_SD_VENC_DACSEL_DA1S_SHIFT) &
                            CSL_VPS_SD_VENC_DACSEL_DA1S_MASK) |
                       ((dacCfg->dac2 <<
                            CSL_VPS_SD_VENC_DACSEL_DA2S_SHIFT) &
                            CSL_VPS_SD_VENC_DACSEL_DA2S_MASK)|
                       ((dacCfg->dac3 <<
                            CSL_VPS_SD_VENC_DACSEL_DA3S_SHIFT) &
                            CSL_VPS_SD_VENC_DACSEL_DA3S_MASK);
    if (TRUE == dacCfg->enableDac0)
    {
        regOvly->DACSEL &= ~CSL_VPS_SD_VENC_DACSEL_DA0E_MASK;
    }
    else
    {
        regOvly->DACSEL |= CSL_VPS_SD_VENC_DACSEL_DA0E_MASK;
    }
    if (TRUE == dacCfg->enableDac1)
    {
        regOvly->DACSEL &= ~CSL_VPS_SD_VENC_DACSEL_DA1E_MASK;
    }
    else
    {
        regOvly->DACSEL |= CSL_VPS_SD_VENC_DACSEL_DA1E_MASK;
    }
    if (TRUE == dacCfg->enableDac2)
    {
        regOvly->DACSEL &= ~CSL_VPS_SD_VENC_DACSEL_DA2E_MASK;
    }
    else
    {
        regOvly->DACSEL |= CSL_VPS_SD_VENC_DACSEL_DA2E_MASK;
    }
    if (TRUE == dacCfg->enableDac3)
    {
        regOvly->DACSEL &= ~CSL_VPS_SD_VENC_DACSEL_DA3E_MASK;
    }
    else
    {
        regOvly->DACSEL |= CSL_VPS_SD_VENC_DACSEL_DA3E_MASK;
    }
    regOvly->DACTST = 0;
    regOvly->DACTST |= ((dacCfg->dclevelControl <<
                            CSL_VPS_SD_VENC_DACTST_DALVL_SHIFT) &
                            CSL_VPS_SD_VENC_DACTST_DALVL_MASK);
    if (VPSHAL_SDVENC_DC_MODE_DC == dacCfg->dcOutputMode)
    {
        regOvly->DACTST |= CSL_VPS_SD_VENC_DACTST_DADC_MASK;
    }
    if (TRUE == dacCfg->invert)
    {
        regOvly->DACTST |= CSL_VPS_SD_VENC_DACTST_DAIV_MASK;
    }
}

Void sdVencCgmsConfig(const VpsHal_SdVencCgmsConfig *cgmsCfg,
                      CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->VIDBCTL =0;
    if (TRUE == cgmsCfg->enable)
    {
        regOvly->VIDBCTL |= ((cgmsCfg->typeBPacketHeader <<
                                CSL_VPS_SD_VENC_VIDBCTL_VIDBH_SHIFT) &
                                CSL_VPS_SD_VENC_VIDBCTL_VIDBH_MASK);
        regOvly->VIDBCTL |= CSL_VPS_SD_VENC_VIDBCTL_VIDB_EN_MASK;

        regOvly->VIDBDATA0 = cgmsCfg->typeBPayLoad0;
        regOvly->VIDBDATA1 = cgmsCfg->typeBPayLoad1;
        regOvly->VIDBDATA2 = cgmsCfg->typeBPayLoad2;
        regOvly->VIDBDATA3 = cgmsCfg->typeBPayLoad3;
    }
}

/**
 * VPsHal_sdVencConfig
 * \brief This function allows the full configuration of the
 * SD Venc
 * \brief handle            Requested SDVENC instance
 * \brief vencCfg           Venc configuration
 */
Void VpsHal_sdVencGetConfig(VpsHal_Handle handle, VpsHal_SdVencConfig *vencCfg)
{
    vpsHal_SdVencDtvConfig *dtvCfg;
    vpsHal_SdVencDvoConfig *dvoCfg;
    VpsHal_SdVencTvDetConfig     *tvDetCfg;
    vpsHal_SdVencSalveConfig *slvCfg;
    vpsHal_SdVencEtmgConfig *encCfg;
    vpsHal_SdVencCvbsConfig *cvbsCfg;
    vpsHal_SdVencComponentConfig *comCfg;
    VpsHal_SdVencClosedCapConfig *ccCfg;
    VpsHal_SdVencWssConfig *wssCfg;
    VpsHal_SdVencCgmsConfig *cgmsCfg;
    SdVenc_Obj              *sdVencObj = NULL;
    CSL_VpsSdVencRegsOvly    regOvly = NULL;

    GT_assert( GT_DEFAULT_MASK, NULL != handle);
    sdVencObj = (SdVenc_Obj *)handle;

    dtvCfg      = &vencCfg->dtvConfig;
    dvoCfg      = &vencCfg->dvoConfig;
    tvDetCfg    = &vencCfg->tvDetectConfig;
    slvCfg      = &vencCfg->slaveConfig;
    encCfg      = &vencCfg->encoderTiming;
    cvbsCfg     = &vencCfg->cvbsConfig;
    comCfg      = &vencCfg->componentConfig;
    ccCfg       = &vencCfg->ccConfig;
    wssCfg      = &vencCfg->wssConfig;
    cgmsCfg     = &vencCfg->cgmsConfig;


    regOvly = sdVencObj->regOvly;

    if (TRUE == (regOvly->VMOD & CSL_VPS_SD_VENC_VMOD_ITLC_MASK))
    {
        vencCfg->scanFormat = VPS_SF_INTERLACED;
    }

    else
    {
        vencCfg->scanFormat = VPS_SF_PROGRESSIVE;
    }
    if (TRUE == (regOvly->VMOD & CSL_VPS_SD_VENC_VMOD_UEL_MASK))
    {
        vencCfg->FieldLineSym = TRUE;
    }
    else
    {
        vencCfg->FieldLineSym = FALSE;
    }
    regOvly->VMOD &= ~CSL_VPS_SD_VENC_VMOD_CBAR_MASK;
    if (TRUE == (regOvly->VMOD & CSL_VPS_SD_VENC_VMOD_DIIV_MASK))
    {
        vencCfg->inputDataInv = TRUE;
    }
    else
    {
        vencCfg->inputDataInv = FALSE;
    }
    if (TRUE == (regOvly->SLAVE & CSL_VPS_SD_VENC_SLAVE_SLV_MASK))
    {
        vencCfg->slave = TRUE;
        sdVencGetSlaveCfg(slvCfg, regOvly);

    }
    else
    {
        vencCfg->slave = FALSE;
    }
    vencCfg->totalLines = (regOvly->SIZE & CSL_VPS_SD_VENC_SIZE_VITV_MASK) >>
                                CSL_VPS_SD_VENC_SIZE_VITV_SHIFT;
    vencCfg->totalPixels = (regOvly->SIZE & CSL_VPS_SD_VENC_SIZE_HITV_MASK) >>
                                CSL_VPS_SD_VENC_SIZE_HITV_SHIFT;

    sdVencGetPolarityConfig(dtvCfg, dvoCfg, tvDetCfg, regOvly);
    sdVencGetDtvConfig(dtvCfg, regOvly);
    sdVencGetDvoConfig(dvoCfg, regOvly);
    sdVencGetTvDetectConfig(tvDetCfg, regOvly);
    vencCfg->irqOutputStartPixel = (regOvly->IRQ0 &
                                        CSL_VPS_SD_VENC_IRQ0_IRQ_H_STA_MASK) >>
                                        CSL_VPS_SD_VENC_IRQ0_IRQ_H_STA_SHIFT;
    vencCfg->irqOutputStartLine = (regOvly->IRQ0 &
                                        CSL_VPS_SD_VENC_IRQ0_IRQ_V_STA_MASK) >>
                                        CSL_VPS_SD_VENC_IRQ0_IRQ_V_STA_SHIFT;

    sdVencGetControlConfig(vencCfg, regOvly);
    sdVencGetEtmgConfig(encCfg, regOvly);
    sdVencGetCvbsConfig(cvbsCfg, regOvly);
    sdVencGetCompConfig(comCfg, regOvly);
    sdVencGetCcConfig(ccCfg, regOvly);
    sdVencGetWssConfig(wssCfg, regOvly);
    sdVencGetCgmsConfig(cgmsCfg, regOvly);

    vencCfg->subCarrierIniPhase = (regOvly->SCCTL0 &
                                    CSL_VPS_SD_VENC_SCCTL0_SCSD_MASK) >>
                                    CSL_VPS_SD_VENC_SCCTL0_SCSD_SHIFT;
    vencCfg->subCarrierFreqPar0 = (regOvly->SCCTL0 &
                                    CSL_VPS_SD_VENC_SCCTL0_SCP0_MASK) >>
                                    CSL_VPS_SD_VENC_SCCTL0_SCP0_SHIFT;
    vencCfg->subCarrierFreqPar1 = (regOvly->SCCTL1 &
                                    CSL_VPS_SD_VENC_SCCTL1_SCP1_MASK) >>
                                    CSL_VPS_SD_VENC_SCCTL1_SCP1_SHIFT;
    vencCfg->subCarrierFreqPar2 = (regOvly->SCCTL1 &
                                    CSL_VPS_SD_VENC_SCCTL1_SCP2_MASK) >>
                                    CSL_VPS_SD_VENC_SCCTL1_SCP2_SHIFT;
}


/* TODO This #defines should be actually part of the CSLR */
#define SDVENC_WSS_WORD0_SHIFT          0x00000000u
#define SDVENC_WSS_WORD1_SHIFT          0x00000002u
#define SDVENC_WSS_WORD2_SHIFT          0x00000006u
#define SDVENC_WSS_CRC_SHIFT            0x0000000Eu
#define SDVENC_WSS_GRP1_SHIFT           0x00000000u
#define SDVENC_WSS_GRP2_SHIFT           0x00000004u
#define SDVENC_WSS_GRP3_SHIFT           0x00000008u
#define SDVENC_WSS_GRP4_SHIFT           0x0000000Bu

#define SDVENC_WSS_WORD0_MASK          0x00000003u
#define SDVENC_WSS_WORD1_MASK          0x0000003Cu
#define SDVENC_WSS_WORD2_MASK          0x00003FC0u
#define SDVENC_WSS_CRC_MASK            0x000FC000u
#define SDVENC_WSS_GRP1_MASK           0x0000000Fu
#define SDVENC_WSS_GRP2_MASK           0x000000F0u
#define SDVENC_WSS_GRP3_MASK           0x00000700u
#define SDVENC_WSS_GRP4_MASK           0x00003800u



Void sdVencWssConfig (const VpsHal_SdVencWssConfig *wssCfg,
                      CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->WSS = 0;
    if (FALSE != wssCfg->enable)
    {
        switch (wssCfg->wssMode)
        {
            case VPSHAL_SDVENC_WSS_MODE0:
                regOvly->WSS |= ((wssCfg->word0 <<
                                    SDVENC_WSS_WORD0_SHIFT) &
                                    SDVENC_WSS_WORD0_MASK) |
                                ((wssCfg->word1 <<
                                    SDVENC_WSS_WORD1_SHIFT) &
                                    SDVENC_WSS_WORD1_MASK) |
                                ((wssCfg->word2 <<
                                    SDVENC_WSS_WORD2_SHIFT) &
                                    SDVENC_WSS_WORD2_MASK) |
                                ((wssCfg->crc <<
                                    SDVENC_WSS_CRC_SHIFT) &
                                    SDVENC_WSS_CRC_MASK);
            break;
            case VPSHAL_SDVENC_WSS_MODE1:
                regOvly->WSS |= ((wssCfg->group1 <<
                                    SDVENC_WSS_GRP1_SHIFT) &
                                    SDVENC_WSS_GRP1_MASK) |
                                ((wssCfg->group2 <<
                                    SDVENC_WSS_GRP2_SHIFT) &
                                    SDVENC_WSS_GRP2_MASK) |
                                ((wssCfg->group3 <<
                                    SDVENC_WSS_GRP3_SHIFT) &
                                    SDVENC_WSS_GRP3_MASK) |
                                ((wssCfg->group4 <<
                                    SDVENC_WSS_GRP4_SHIFT) &
                                    SDVENC_WSS_GRP4_MASK);
            break;
        }
        regOvly->WSS |= CSL_VPS_SD_VENC_WSS_WSS_EN_MASK;
    }
}

Void sdVencCcConfig(const VpsHal_SdVencClosedCapConfig *ccCfg,
                    CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->L21CTL = 0;
    switch (ccCfg->ccSelection)
    {
        case VPSHAL_SDVENC_CC_NO_DATA:
            regOvly->L21CTL |= ((VPSHAL_SDVENC_CC_NO_DATA  <<
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_SHIFT) &
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_MASK);
        break;
        case VPSHAL_SDVENC_CC_ODD_FIELD:
            regOvly->L21CTL |= ((VPSHAL_SDVENC_CC_ODD_FIELD  <<
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_SHIFT) &
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_MASK);
        break;
        case VPSHAL_SDVENC_CC_EVEN_FIELD:
            regOvly->L21CTL |= ((VPSHAL_SDVENC_CC_EVEN_FIELD  <<
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_SHIFT) &
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_MASK);
        break;
        case VPSHAL_SDVENC_CC_BOTH_FIELDS:
            regOvly->L21CTL |= ((VPSHAL_SDVENC_CC_BOTH_FIELDS  <<
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_SHIFT) &
                                    CSL_VPS_SD_VENC_L21CTL_L21EN_MASK);
        break;
    }
    regOvly->L21CTL |= ((ccCfg->ccDefaultData <<
                            CSL_VPS_SD_VENC_L21CTL_L21DF_SHIFT) &
                            CSL_VPS_SD_VENC_L21CTL_L21DF_MASK);
    regOvly->L21DO = ((ccCfg->ccData0[1] <<
                            CSL_VPS_SD_VENC_L21DO_L21DO1_SHIFT) &
                            CSL_VPS_SD_VENC_L21DO_L21DO1_MASK) |
                     ((ccCfg->ccData0[0] <<
                            CSL_VPS_SD_VENC_L21DO_L21DO0_SHIFT) &
                            CSL_VPS_SD_VENC_L21DO_L21DO0_MASK);
    regOvly->L21DE == ((ccCfg->ccData1[1] <<
                            CSL_VPS_SD_VENC_L21DE_L21DE1_SHIFT) &
                            CSL_VPS_SD_VENC_L21DE_L21DE1_MASK) |
                       ((ccCfg->ccData1[0] <<
                            CSL_VPS_SD_VENC_L21DE_L21DE0_SHIFT) &
                            CSL_VPS_SD_VENC_L21DE_L21DE0_MASK);
}

Void sdVencGetDacConfig(VpsHal_SdVencDacConfig *dacCfg,
                     CSL_VpsSdVencRegsOvly regOvly)
{
    dacCfg->dac0 = (VpsHal_SdVencDacSelection)
                        ((regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA0S_MASK) >>
                        CSL_VPS_SD_VENC_DACSEL_DA0S_SHIFT);
    dacCfg->dac1 = (VpsHal_SdVencDacSelection)
                        ((regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA1S_MASK) >>
                        CSL_VPS_SD_VENC_DACSEL_DA1S_SHIFT);
    dacCfg->dac2 = (VpsHal_SdVencDacSelection)
                        ((regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA2S_MASK) >>
                        CSL_VPS_SD_VENC_DACSEL_DA2S_SHIFT);
    dacCfg->dac3 = (VpsHal_SdVencDacSelection)
                        ((regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA3S_MASK) >>
                        CSL_VPS_SD_VENC_DACSEL_DA3S_SHIFT);

    dacCfg->enableDac0 = (regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA0E_MASK)
                            >> CSL_VPS_SD_VENC_DACSEL_DA0E_SHIFT;
    dacCfg->enableDac1 = (regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA1E_MASK)
                            >> CSL_VPS_SD_VENC_DACSEL_DA1E_SHIFT;
    dacCfg->enableDac2 = (regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA2E_MASK)
                            >> CSL_VPS_SD_VENC_DACSEL_DA2E_SHIFT;
    dacCfg->enableDac3 = (regOvly->DACSEL & CSL_VPS_SD_VENC_DACSEL_DA3E_MASK)
                            >> CSL_VPS_SD_VENC_DACSEL_DA3E_SHIFT;

    /* Since enabled bit is inverted in SDDAC, invert this flag as well */
    dacCfg->enableDac0 = dacCfg->enableDac0 ? 0: 1;
    dacCfg->enableDac1 = dacCfg->enableDac1 ? 0: 1;
    dacCfg->enableDac2 = dacCfg->enableDac2 ? 0: 1;
    dacCfg->enableDac3 = dacCfg->enableDac3 ? 0: 1;

    dacCfg->dclevelControl = (regOvly->DACTST &
                                CSL_VPS_SD_VENC_DACTST_DALVL_MASK) >>
                                CSL_VPS_SD_VENC_DACTST_DALVL_SHIFT;

    dacCfg->dcOutputMode = (VpsHal_SdVencDacOutMode)
                    ((regOvly->DACTST & CSL_VPS_SD_VENC_DACTST_DADC_MASK) >>
                        CSL_VPS_SD_VENC_DACTST_DADC_SHIFT);

    dacCfg->invert = (regOvly->DACTST & CSL_VPS_SD_VENC_DACTST_DAIV_MASK) >>
                        CSL_VPS_SD_VENC_DACTST_DAIV_SHIFT;
}

Void sdVencGetCgmsConfig(VpsHal_SdVencCgmsConfig *cgmsCfg,
                      CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->VIDBCTL =0;
    if (TRUE == regOvly->VIDBCTL & CSL_VPS_SD_VENC_VIDBCTL_VIDB_EN_MASK)
    {
        cgmsCfg->enable = TRUE;
        cgmsCfg->typeBPacketHeader = regOvly->VIDBCTL &
                                        (CSL_VPS_SD_VENC_VIDBCTL_VIDBH_MASK) >>
                                        CSL_VPS_SD_VENC_VIDBCTL_VIDBH_SHIFT;

        cgmsCfg->typeBPayLoad0 = regOvly->VIDBDATA0;
        cgmsCfg->typeBPayLoad1 = regOvly->VIDBDATA1;
        cgmsCfg->typeBPayLoad2 = regOvly->VIDBDATA2;
        cgmsCfg->typeBPayLoad3 = regOvly->VIDBDATA3;
    }
}

/* TODO This #defines should be actually part of the CSLR */
#define SDVENC_WSS_WORD0_SHIFT          0x00000000u
#define SDVENC_WSS_WORD1_SHIFT          0x00000002u
#define SDVENC_WSS_WORD2_SHIFT          0x00000006u
#define SDVENC_WSS_CRC_SHIFT            0x0000000Eu
#define SDVENC_WSS_GRP1_SHIFT           0x00000000u
#define SDVENC_WSS_GRP2_SHIFT           0x00000004u
#define SDVENC_WSS_GRP3_SHIFT           0x00000008u
#define SDVENC_WSS_GRP4_SHIFT           0x0000000Bu

#define SDVENC_WSS_WORD0_MASK          0x00000003u
#define SDVENC_WSS_WORD1_MASK          0x0000003Cu
#define SDVENC_WSS_WORD2_MASK          0x00003FC0u
#define SDVENC_WSS_CRC_MASK            0x000FC000u
#define SDVENC_WSS_GRP1_MASK           0x0000000Fu
#define SDVENC_WSS_GRP2_MASK           0x000000F0u
#define SDVENC_WSS_GRP3_MASK           0x00000700u
#define SDVENC_WSS_GRP4_MASK           0x00003800u



Void sdVencGetWssConfig (VpsHal_SdVencWssConfig *wssCfg,
                      CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->WSS = 0;
    if (TRUE == (regOvly->WSS & CSL_VPS_SD_VENC_WSS_WSS_EN_MASK))
    {
        wssCfg->enable = TRUE;
    }
    else
    {
        wssCfg->enable = FALSE;
    }
}

Void sdVencGetCcConfig(VpsHal_SdVencClosedCapConfig *ccCfg,
                    CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->L21CTL = 0;
    switch ((regOvly->L21CTL & CSL_VPS_SD_VENC_L21CTL_L21EN_MASK) >>
                CSL_VPS_SD_VENC_L21CTL_L21EN_SHIFT)
    {
        case VPSHAL_SDVENC_CC_NO_DATA:
             ccCfg->ccSelection = VPSHAL_SDVENC_CC_NO_DATA;
        break;
        case VPSHAL_SDVENC_CC_ODD_FIELD:
            ccCfg->ccSelection = VPSHAL_SDVENC_CC_ODD_FIELD;
        break;
        case VPSHAL_SDVENC_CC_EVEN_FIELD:
            ccCfg->ccSelection = VPSHAL_SDVENC_CC_EVEN_FIELD;
        break;
        case VPSHAL_SDVENC_CC_BOTH_FIELDS:
            ccCfg->ccSelection = VPSHAL_SDVENC_CC_BOTH_FIELDS;
        break;
    }
    ccCfg->ccDefaultData = (regOvly->L21CTL &
                                CSL_VPS_SD_VENC_L21CTL_L21DF_MASK) >>
                                CSL_VPS_SD_VENC_L21CTL_L21DF_SHIFT;
    ccCfg->ccData0[1] =  (regOvly->L21DO &
                                CSL_VPS_SD_VENC_L21DO_L21DO1_MASK) >>
                                CSL_VPS_SD_VENC_L21DO_L21DO1_SHIFT;
    ccCfg->ccData0[0] =  (regOvly->L21DO &
                                CSL_VPS_SD_VENC_L21DO_L21DO0_MASK) >>
                                CSL_VPS_SD_VENC_L21DO_L21DO0_SHIFT;
    ccCfg->ccData1[1] = (regOvly->L21DE &
                                CSL_VPS_SD_VENC_L21DE_L21DE1_MASK >>
                                CSL_VPS_SD_VENC_L21DE_L21DE1_SHIFT);
    ccCfg->ccData1[0] = (regOvly->L21DE &
                                CSL_VPS_SD_VENC_L21DE_L21DE0_MASK) >>
                                CSL_VPS_SD_VENC_L21DE_L21DE0_SHIFT;
}


Void sdVencSet2xSamCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly)
{
    VpsHal_SdVenc2xUpSampCoeff *coeffPtr;

    coeffPtr = vencCfg->upSamplingCoeffs;
    if (NULL == coeffPtr)
    {
        coeffPtr = &defaultLuma2xUpSampCoeffs;
    }
    regOvly->UPF0 = ((coeffPtr->coeffs[0] <<
                        CSL_VPS_SD_VENC_UPF0_UPFC0_SHIFT) &
                        CSL_VPS_SD_VENC_UPF0_UPFC0_MASK) |
                    ((coeffPtr->coeffs[1] <<
                        CSL_VPS_SD_VENC_UPF0_UPFC1_SHIFT) &
                        CSL_VPS_SD_VENC_UPF0_UPFC1_MASK) |
                    ((coeffPtr->coeffs[2] <<
                        CSL_VPS_SD_VENC_UPF0_UPFC2_SHIFT) &
                        CSL_VPS_SD_VENC_UPF0_UPFC2_MASK) |
                    ((coeffPtr->coeffs[3] <<
                        CSL_VPS_SD_VENC_UPF0_UPFC3_SHIFT) &
                        CSL_VPS_SD_VENC_UPF0_UPFC3_MASK);
    regOvly->UPF1 = ((coeffPtr->coeffs[4] <<
                        CSL_VPS_SD_VENC_UPF1_UPFC4_SHIFT) &
                        CSL_VPS_SD_VENC_UPF1_UPFC4_MASK) |
                    ((coeffPtr->coeffs[5] <<
                        CSL_VPS_SD_VENC_UPF1_UPFC5_SHIFT) &
                        CSL_VPS_SD_VENC_UPF1_UPFC5_MASK) |
                    ((coeffPtr->coeffs[6] <<
                        CSL_VPS_SD_VENC_UPF1_UPFC6_SHIFT) &
                        CSL_VPS_SD_VENC_UPF1_UPFC6_MASK);
}

Void sdVencSet2xOverSamCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly)
{
    VpsHal_SdVencDac2xOverSampCoeff *coeffPtr;

    coeffPtr = vencCfg->dacOverSamplingCoeffs;
    if (NULL == coeffPtr)
    {
        coeffPtr = &defaultDac2xOvrSampCoeffs;
    }
    regOvly->DUPF0 = ((coeffPtr->coeffs[0] <<
                        CSL_VPS_SD_VENC_DUPF0_DUPFC0_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF0_DUPFC0_MASK) |
                    ((coeffPtr->coeffs[1] <<
                        CSL_VPS_SD_VENC_DUPF0_DUPFC1_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF0_DUPFC1_MASK) |
                    ((coeffPtr->coeffs[2] <<
                        CSL_VPS_SD_VENC_DUPF0_DUPFC2_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF0_DUPFC2_MASK) |
                    ((coeffPtr->coeffs[3] <<
                        CSL_VPS_SD_VENC_DUPF0_DUPFC3_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF0_DUPFC3_MASK);
    regOvly->DUPF1 = ((coeffPtr->coeffs[4] <<
                        CSL_VPS_SD_VENC_DUPF1_DUPFC4_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF1_DUPFC4_MASK) |
                    ((coeffPtr->coeffs[5] <<
                        CSL_VPS_SD_VENC_DUPF1_DUPFC5_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF1_DUPFC5_MASK) |
                    ((coeffPtr->coeffs[6] <<
                        CSL_VPS_SD_VENC_DUPF1_DUPFC6_SHIFT) &
                        CSL_VPS_SD_VENC_DUPF1_DUPFC6_MASK);
}

Void sdVencSetLpfCoeffs(VpsHal_SdVencConfig *vencCfg,
                        CSL_VpsSdVencRegsOvly regOvly)
{
    VpsHal_SdVencLpfCoeff *coeffPtr;

    coeffPtr = vencCfg->lumaLpfCoeffs;
    if (NULL == vencCfg->lumaLpfCoeffs)
    {
        coeffPtr = &defaultLumaLpfCoeffs;
    }
    regOvly->YLPF0 = ((coeffPtr->coeffs[0] <<
                        CSL_VPS_SD_VENC_YLPF0_YLPFC0_SHIFT) &
                        CSL_VPS_SD_VENC_YLPF0_YLPFC0_MASK) |
                     ((coeffPtr->coeffs[1] <<
                        CSL_VPS_SD_VENC_YLPF0_YLPFC1_SHIFT) &
                        CSL_VPS_SD_VENC_YLPF0_YLPFC1_MASK) |
                     ((coeffPtr->coeffs[2] <<
                        CSL_VPS_SD_VENC_YLPF0_YLPFC2_SHIFT) &
                        CSL_VPS_SD_VENC_YLPF0_YLPFC2_MASK) |
                     ((coeffPtr->coeffs[3] <<
                        CSL_VPS_SD_VENC_YLPF0_YLPFC3_SHIFT) &
                        CSL_VPS_SD_VENC_YLPF0_YLPFC3_MASK);
    regOvly->YLPF1 = ((coeffPtr->coeffs[4] <<
                        CSL_VPS_SD_VENC_YLPF1_YLPFC4_SHIFT) &
                        CSL_VPS_SD_VENC_YLPF1_YLPFC4_MASK) |
                      ((coeffPtr->coeffs[5] <<
                        CSL_VPS_SD_VENC_YLPF1_YLPFC5_SHIFT) &
                        CSL_VPS_SD_VENC_YLPF1_YLPFC5_MASK);

    coeffPtr = vencCfg->chromaLpfCoeffs;
    if (NULL == vencCfg->chromaLpfCoeffs)
    {
        coeffPtr = &defaultChromaLpfCoeffs;
    }
    regOvly->CLPF0 = ((coeffPtr->coeffs[0] <<
                        CSL_VPS_SD_VENC_CLPF0_CLPFC0_SHIFT) &
                        CSL_VPS_SD_VENC_CLPF0_CLPFC0_MASK) |
                     ((coeffPtr->coeffs[1] <<
                        CSL_VPS_SD_VENC_CLPF0_CLPFC1_SHIFT) &
                        CSL_VPS_SD_VENC_CLPF0_CLPFC1_MASK) |
                     ((coeffPtr->coeffs[2] <<
                        CSL_VPS_SD_VENC_CLPF0_CLPFC2_SHIFT) &
                        CSL_VPS_SD_VENC_CLPF0_CLPFC2_MASK) |
                     ((coeffPtr->coeffs[3] <<
                        CSL_VPS_SD_VENC_CLPF0_CLPFC3_SHIFT) &
                        CSL_VPS_SD_VENC_CLPF0_CLPFC3_MASK);
    regOvly->CLPF1 = ((coeffPtr->coeffs[4] <<
                        CSL_VPS_SD_VENC_CLPF1_CLPFC4_SHIFT) &
                        CSL_VPS_SD_VENC_CLPF1_CLPFC4_MASK) |
                      ((coeffPtr->coeffs[5] <<
                        CSL_VPS_SD_VENC_CLPF1_CLPFC5_SHIFT) &
                        CSL_VPS_SD_VENC_CLPF1_CLPFC5_MASK);
}

Void sdVencSetCompConfig(const vpsHal_SdVencComponentConfig *comCfg,
                         CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->CMPNT0 = ((comCfg->synctipAmp <<
                            CSL_VPS_SD_VENC_CMPNT0_MTLVL_SHIFT) &
                            CSL_VPS_SD_VENC_CMPNT0_MTLVL_MASK) |
                        ((comCfg->syncAmp <<
                            CSL_VPS_SD_VENC_CMPNT0_MSLVL_SHIFT) &
                            CSL_VPS_SD_VENC_CMPNT0_MSLVL_MASK);
    regOvly->CMPNT1 = 0u;
    switch (comCfg->pictSynRatio)
    {
        case VPSHAL_SDVENC_PIC_SYNC_RAT_10_4:
            regOvly->CMPNT1 &=  ~CSL_VPS_SD_VENC_CMPNT1_MPSR_MASK;
        break;
        case VPSHAL_SDVENC_PIC_SYNC_RAT_7_3:
            regOvly->CMPNT1 |=  CSL_VPS_SD_VENC_CMPNT1_MPSR_MASK;
        break;
    }
    switch (comCfg->outColorSpace)
    {
        case VPSHAL_SDVENC_OUTPUT_RGB:
            regOvly->CMPNT1 |= CSL_VPS_SD_VENC_CMPNT1_MRGB_MASK;
        break;
        case VPSHAL_SDVENC_OUTPUT_YUV:
            regOvly->CMPNT1 &= ~CSL_VPS_SD_VENC_CMPNT1_MRGB_MASK;
        break;
    }
    if (TRUE == comCfg->syncOnYOrG)
    {
        regOvly->CMPNT1 |= CSL_VPS_SD_VENC_CMPNT1_MSOYG_MASK;
    }
    else
    {
        regOvly->CMPNT1 &= ~CSL_VPS_SD_VENC_CMPNT1_MSOYG_MASK;
    }
    if (TRUE == comCfg->syncOnPbOrB)
    {
        regOvly->CMPNT1 |= CSL_VPS_SD_VENC_CMPNT1_MSOUB_MASK;
    }
    else
    {
        regOvly->CMPNT1 &= ~CSL_VPS_SD_VENC_CMPNT1_MSOUB_MASK;
    }
    if (TRUE == comCfg->syncOnPrOrR)
    {
        regOvly->CMPNT1 |= CSL_VPS_SD_VENC_CMPNT1_MSOVR_MASK;
    }
    else
    {
        regOvly->CMPNT1 &= ~CSL_VPS_SD_VENC_CMPNT1_MSOVR_MASK;

    }
    regOvly->MYGCLP = ((comCfg->yGUpperLimit <<
                            CSL_VPS_SD_VENC_MYGCLP_MYGUCLP_SHIFT) &
                            CSL_VPS_SD_VENC_MYGCLP_MYGUCLP_MASK) |
                       ((comCfg->yGLowerLimit <<
                            CSL_VPS_SD_VENC_MYGCLP_MYGLCLP_SHIFT) &
                            CSL_VPS_SD_VENC_MYGCLP_MYGLCLP_MASK);
    regOvly->MUBCLP = ((comCfg->uBUpperLimit <<
                            CSL_VPS_SD_VENC_MUBCLP_MUBUCLP_SHIFT) &
                            CSL_VPS_SD_VENC_MUBCLP_MUBUCLP_MASK) |
                       ((comCfg->uBLowerLimit <<
                            CSL_VPS_SD_VENC_MUBCLP_MUBLCLP_SHIFT) &
                            CSL_VPS_SD_VENC_MUBCLP_MUBLCLP_MASK);
    regOvly->MVRCLP == ((comCfg->vRUpperLimit <<
                            CSL_VPS_SD_VENC_MVRCLP_MVRUCLP_SHIFT) &
                            CSL_VPS_SD_VENC_MVRCLP_MVRUCLP_MASK) |
                       ((comCfg->vRLowerLimit <<
                            CSL_VPS_SD_VENC_MVRCLP_MVRLCLP_SHIFT) &
                            CSL_VPS_SD_VENC_MVRCLP_MVRLCLP_MASK);
}

Void sdVencSetCvbsConfig(const vpsHal_SdVencCvbsConfig *cvbsCfg,
                         CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->CVBS0 = ((cvbsCfg->synctipAmp <<
                        CSL_VPS_SD_VENC_CVBS0_CTLVL_SHIFT) &
                        CSL_VPS_SD_VENC_CVBS0_CTLVL_MASK) |
                      ((cvbsCfg->syncAmp <<
                        CSL_VPS_SD_VENC_CVBS0_CSLVL_SHIFT) &
                        CSL_VPS_SD_VENC_CVBS0_CSLVL_MASK);
    regOvly->CVBS1 = 0u;

    switch (cvbsCfg->pictSynRatio)
    {
        case VPSHAL_SDVENC_PIC_SYNC_RAT_10_4:
            regOvly->CVBS1 &=  ~CSL_VPS_SD_VENC_CVBS1_CPSR_MASK;
        break;
        case VPSHAL_SDVENC_PIC_SYNC_RAT_7_3:
            regOvly->CVBS1 |=  CSL_VPS_SD_VENC_CVBS1_CPSR_MASK;
        break;
    }
    if (TRUE == cvbsCfg->lumaLpfEnable)
    {
        regOvly->CVBS1 |= CSL_VPS_SD_VENC_CVBS1_YLPF_MASK;
    }
    else
    {
        regOvly->CVBS1 &= ~CSL_VPS_SD_VENC_CVBS1_YLPF_MASK;
    }
    if (TRUE == cvbsCfg->chromaLpfEnable)
    {
        regOvly->CVBS1 |= CSL_VPS_SD_VENC_CVBS1_CLPF_MASK;
    }
    else
    {
        regOvly->CVBS1 &= ~CSL_VPS_SD_VENC_CVBS1_CLPF_MASK;
    }
    switch (cvbsCfg->colorModulation)
    {
        case VPSHAL_SDVENC_COL_MODULATION_MODE_NTSC:
            regOvly->CVBS1 |= ((VPSHAL_SDVENC_COL_MODULATION_MODE_NTSC  <<
                            CSL_VPS_SD_VENC_CVBS1_CCM_SHIFT) &
                            CSL_VPS_SD_VENC_CVBS1_CCM_MASK);
        break;
        case VPSHAL_SDVENC_COL_MODULATION_MODE_PAL:
            regOvly->CVBS1 |= ((VPSHAL_SDVENC_COL_MODULATION_MODE_PAL  <<
                            CSL_VPS_SD_VENC_CVBS1_CCM_SHIFT) &
                            CSL_VPS_SD_VENC_CVBS1_CCM_MASK);
        break;
        case VPSHAL_SDVENC_COL_MODULATION_MODE_SECAM:
            regOvly->CVBS1 |= ((VPSHAL_SDVENC_COL_MODULATION_MODE_SECAM  <<
                            CSL_VPS_SD_VENC_CVBS1_CCM_SHIFT) &
                            CSL_VPS_SD_VENC_CVBS1_CCM_MASK);
        break;
    }
    regOvly->CVBS1 |= ((cvbsCfg->yDelayAdjustment <<
                            CSL_VPS_SD_VENC_CVBS1_CYDLY_SHIFT) &
                            CSL_VPS_SD_VENC_CVBS1_CYDLY_MASK) |
                       ((cvbsCfg->burstAmplitude <<
                            CSL_VPS_SD_VENC_CVBS1_CBLVL_SHIFT) &
                            CSL_VPS_SD_VENC_CVBS1_CBLVL_MASK);
    regOvly->CYGCLP = ((cvbsCfg->yClipUpperLimit <<
                            CSL_VPS_SD_VENC_CYGCLP_CYUCLP_SHIFT) &
                            CSL_VPS_SD_VENC_CYGCLP_CYUCLP_MASK) |
                        ((cvbsCfg->yClipLowerLimit <<
                            CSL_VPS_SD_VENC_CYGCLP_CYLCLP_SHIFT) &
                            CSL_VPS_SD_VENC_CYGCLP_CYLCLP_MASK);
    regOvly->CUBCLP = ((cvbsCfg->uClipUpperLimit <<
                            CSL_VPS_SD_VENC_CUBCLP_CUUCLP_SHIFT) &
                            CSL_VPS_SD_VENC_CUBCLP_CUUCLP_MASK) |
                      ((cvbsCfg->uClipLowerLimit <<
                            CSL_VPS_SD_VENC_CUBCLP_CULCLP_SHIFT) &
                            CSL_VPS_SD_VENC_CUBCLP_CULCLP_MASK);
    regOvly->CVRCLP == ((cvbsCfg->vClipUpperLimit <<
                            CSL_VPS_SD_VENC_CVRCLP_CVUCLP_SHIFT) &
                            CSL_VPS_SD_VENC_CVRCLP_CVUCLP_MASK) |
                        ((cvbsCfg->vClipLowerLimit <<
                            CSL_VPS_SD_VENC_CVRCLP_CVLCLP_SHIFT) &
                            CSL_VPS_SD_VENC_CVRCLP_CVLCLP_MASK);
}


Void sdVencSetEtmgConfig(const vpsHal_SdVencEtmgConfig *eTmgCfg,
                         CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->ETMG0 = ((eTmgCfg->activeVidHorzStartPos <<
                        CSL_VPS_SD_VENC_ETMG0_AV_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG0_AV_H_STA_MASK) |
                     ((eTmgCfg->activeVidHorzStopPos <<
                        CSL_VPS_SD_VENC_ETMG0_AV_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG0_AV_H_STP_MASK);
    regOvly->ETMG1 = ((eTmgCfg->activeVidVertStarPos0 <<
                        CSL_VPS_SD_VENC_ETMG1_AV_V_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG1_AV_V_STA0_MASK) |
                     ((eTmgCfg->activeVidVertStopPos0 <<
                        CSL_VPS_SD_VENC_ETMG1_AV_V_STP0_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG1_AV_V_STP0_MASK);
    regOvly->ETMG2 = ((eTmgCfg->activeVidVertStarPos1 <<
                        CSL_VPS_SD_VENC_ETMG2_AV_V_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG2_AV_V_STA1_MASK) |
                      ((eTmgCfg->activeVidVertStopPos1 <<
                         CSL_VPS_SD_VENC_ETMG2_AV_V_STP1_SHIFT) &
                         CSL_VPS_SD_VENC_ETMG2_AV_V_STP1_MASK);
    regOvly->ETMG3 = ((eTmgCfg->colorBurstStartPos <<
                        CSL_VPS_SD_VENC_ETMG3_BST_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG3_BST_H_STA_MASK) |
                      ((eTmgCfg->colorBurstStopPos <<
                        CSL_VPS_SD_VENC_ETMG3_BST_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG3_BST_H_STP_MASK);
    regOvly->ETMG4 = ((eTmgCfg->vbiReqStartPos <<
                        CSL_VPS_SD_VENC_ETMG4_VBI_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_ETMG4_VBI_H_STA_MASK) |
                      ((eTmgCfg->vbiReqStopPos <<
                         CSL_VPS_SD_VENC_ETMG4_VBI_H_STP_SHIFT) &
                         CSL_VPS_SD_VENC_ETMG4_VBI_H_STP_MASK);
}

Void sdVencSetControlConfig(VpsHal_SdVencConfig *vencCfg,
                            CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->ECTL = 0;
    if (TRUE == vencCfg->gammaCorrEnable)
    {
        regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_GAM_MASK;
    }
    else
    {
        regOvly->ECTL &= ~CSL_VPS_SD_VENC_ECTL_GAM_MASK;
    }
    if (TRUE == vencCfg->Upsampling2XEnable)
    {
        regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_UPS_MASK;
    }
    else
    {
        regOvly->ECTL &= ~CSL_VPS_SD_VENC_ECTL_UPS_MASK;
    }
    if (TRUE == vencCfg->Dac2xOverSamplingEnable)
    {
        regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_DUPS_MASK;
    }
    else
    {
        regOvly->ECTL &= CSL_VPS_SD_VENC_ECTL_DUPS_MASK;
    }
    switch (vencCfg->vertSyncWidth)
    {
        case VPSHAL_SDVENC_VERT_SYNC_WID_30:
            regOvly->ECTL &= ~CSL_VPS_SD_VENC_ECTL_SVSW_MASK;
        break;
        case VPSHAL_SDVENC_VERT_SYNC_WID_25:
            regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_SVSW_MASK;
        break;
    }
    if (TRUE == vencCfg->blankingShapeEnable)
    {
        regOvly->ECTL &= ~CSL_VPS_SD_VENC_ECTL_BLS_MASK;
    }
    else
    {
        regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_BLS_MASK;
    }
    if (TRUE == vencCfg->vbiEnable)
    {
        regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_VBIEN_MASK;
    }
    else
    {
        regOvly->ECTL &= ~CSL_VPS_SD_VENC_ECTL_VBIEN_MASK;
    }
    switch (vencCfg->pixelRateMultiplier)
    {
        case VPSHAL_SDVENC_PIX_CLK_1X:
            regOvly->ECTL &=  ~CSL_VPS_SD_VENC_ECTL_PXLR_MASK;
        break;
        case VPSHAL_SDVENC_PIX_CLK_2X:
            regOvly->ECTL |= CSL_VPS_SD_VENC_ECTL_PXLR_MASK;
        break;
    }
    switch (vencCfg->fmt)
    {
        case VPSHAL_SDVENC_FMT_525I:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_525I <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
        case VPSHAL_SDVENC_FMT_625I:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_625I <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
        case VPSHAL_SDVENC_FMT_525P:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_525P <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
        case VPSHAL_SDVENC_FMT_625P:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_625P <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
        case VPSHAL_SDVENC_FMT_1080I:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_1080I <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
        case VPSHAL_SDVENC_FMT_720P:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_720P <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
        case VPSHAL_SDVENC_FMT_1080P:
            regOvly->ECTL |= ((VPSHAL_SDVENC_FMT_1080P <<
                                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT) &
                                CSL_VPS_SD_VENC_ECTL_FMT_MASK);
        break;
    }
}

Void sdVencSetDtvConfig(const vpsHal_SdVencDtvConfig *dtvCfg,
                       CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->DTVS0 = ((dtvCfg->hSyncStartPixel <<
                        CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STA_MASK) |
                     ((dtvCfg->hSyncStopPixel <<
                        CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STP_MASK);
    regOvly->DTVS1 = ((dtvCfg->vSyncStartPixel <<
                        CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STA_MASK) |
                     ((dtvCfg->vSyncStopPixel <<
                        CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STP_MASK);
    regOvly->DTVS2 = ((dtvCfg->vSyncStartLine <<
                        CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STA_MASK) |
                      ((dtvCfg->vSyncStopLine <<
                        CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STP_MASK);
    regOvly->DTVS3 = ((dtvCfg->fidTogglePixelPosition <<
                        CSL_VPS_SD_VENC_DTVS3_DTV_FID_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS3_DTV_FID_H_STA_MASK);
    regOvly->DTVS4 = ((dtvCfg->fidStartLine0 <<
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA0_MASK) |
                      ((dtvCfg->fidId0 <<
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA0_MASK) |
                      ((dtvCfg->fidStartLine1 <<
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA1_MASK) |
                      ((dtvCfg->fidId1 <<
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA1_MASK);
    regOvly->DTVS5 = ((dtvCfg->avidStartPixel <<
                        CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STA_MASK) |
                      ((dtvCfg->avidStopPixel <<
                        CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STP_MASK);
    regOvly->DTVS6 = ((dtvCfg->avidStartLine0 <<
                        CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STA0_MASK) |
                      ((dtvCfg->avidStopLine0 <<
                        CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STP0_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STP0_MASK);
    regOvly->DTVS7 = ((dtvCfg->avidStartLine1 <<
                        CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STA1_MASK) |
                      ((dtvCfg->avidStopLine1 <<
                        CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STP1_SHIFT) &
                        CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STP1_MASK);
}

Void sdVencSetDvoConfig(const vpsHal_SdVencDvoConfig *dvoCfg,
                       CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->DVOS0 = ((dvoCfg->hSyncStartPixel <<
                        CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STA_MASK) |
                     ((dvoCfg->hSyncStopPixel <<
                        CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STP_MASK);
    regOvly->DVOS1 = ((dvoCfg->vSyncStartPixel <<
                        CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STA_MASK) |
                     ((dvoCfg->vSyncStopPixel <<
                        CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STP_MASK);
    regOvly->DVOS2 = ((dvoCfg->vSyncStartLine <<
                        CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STA_MASK) |
                      ((dvoCfg->vSyncStopLine <<
                        CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STP_MASK);
    regOvly->DVOS3 = ((dvoCfg->fidTogglePixelPosition <<
                        CSL_VPS_SD_VENC_DVOS3_DVO_FID_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS3_DVO_FID_H_STA_MASK) |
                      ((dvoCfg->horzDelay <<
                        CSL_VPS_SD_VENC_DVOS3_DVO_DELAY_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS3_DVO_DELAY_MASK);
    regOvly->DVOS4 = ((dvoCfg->fidStartLine0 <<
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA0_MASK) |
                      ((dvoCfg->fidId0 <<
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA0_MASK) |
                      ((dvoCfg->fidStartLine1 <<
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA1_MASK) |
                      ((dvoCfg->fidId1 <<
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA1_MASK);
    regOvly->DVOS5 = ((dvoCfg->avidStartPixel <<
                        CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STA_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STA_MASK) |
                      ((dvoCfg->avidStopPixel <<
                        CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STP_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STP_MASK);
    regOvly->DVOS6 = ((dvoCfg->avidStartLine0 <<
                        CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STA0_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STA0_MASK) |
                      ((dvoCfg->avidStopLine0 <<
                        CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STP0_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STP0_MASK);
    regOvly->DVOS7 = ((dvoCfg->avidStartLine1 <<
                        CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STA1_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STA1_MASK) |
                      ((dvoCfg->avidStopLine1 <<
                        CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STP1_SHIFT) &
                        CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STP1_MASK);
}

Void sdVencTvDetectConfig(const VpsHal_SdVencTvDetConfig *tvDetCfg,
                          CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->TVDETGP0 = ((tvDetCfg->tvDeteStartPixel <<
                          CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STA_SHIFT) &
                          CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STA_MASK) |
                        ((tvDetCfg->tvDeteStopPixel <<
                          CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STP_SHIFT) &
                          CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STP_MASK);
    regOvly->TVDETGP1 = ((tvDetCfg->tvDeteStartLine <<
                            CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STA_SHIFT) &
                            CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STA_MASK) |
                        ((tvDetCfg->teDeteStopLine <<
                            CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STP_SHIFT) &
                            CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STP_MASK);
}

Void sdVencSetPolarityConfig(const vpsHal_SdVencDtvConfig *dtvCfg,
                             const vpsHal_SdVencDvoConfig *dvoCfg,
                             const VpsHal_SdVencTvDetConfig *tvDetCfg,
                             const CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->POL = 0;

    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dtvCfg->hsPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DTV_HS_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DTV_HS_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dtvCfg->vsPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DTV_VS_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DTV_VS_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dtvCfg->fidPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DTV_FID_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DTV_FID_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dtvCfg->activVidPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DTV_AVID_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DTV_AVID_POL_MASK;
    }

    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dvoCfg->hsPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DVO_HS_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DVO_HS_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dvoCfg->vsPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DVO_VS_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DVO_VS_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dvoCfg->fidPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DVO_FID_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DVO_FID_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == dvoCfg->activVidPolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_DVO_AVID_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_DVO_AVID_POL_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == tvDetCfg->tvDetePolarity)
    {
        regOvly->POL |= CSL_VPS_SD_VENC_POL_TVDETGP_POL_MASK;
    }
    else
    {
        regOvly->POL &= ~CSL_VPS_SD_VENC_POL_TVDETGP_POL_MASK;
    }
}

Void sdVencSetSlaveCfg(const vpsHal_SdVencSalveConfig *slvCfg,
                        CSL_VpsSdVencRegsOvly    regOvly)
{
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == slvCfg->hsPolarity)
    {
        regOvly->SLAVE |= CSL_VPS_SD_VENC_SLAVE_HIP_MASK;
    }
    else
    {
        regOvly->SLAVE &= ~CSL_VPS_SD_VENC_SLAVE_HIP_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == slvCfg->vsPolarity)
    {
        regOvly->SLAVE |= CSL_VPS_SD_VENC_SLAVE_VIP_MASK;
    }
    else
    {
        regOvly->SLAVE &= ~CSL_VPS_SD_VENC_SLAVE_VIP_MASK;
    }
    if (VPSHAL_SDVENC_POLARITY_ACTIVE_LOW == slvCfg->fidPolarity)
    {
        regOvly->SLAVE |= CSL_VPS_SD_VENC_SLAVE_FIP_MASK;
    }
    else
    {
        regOvly->SLAVE &= ~CSL_VPS_SD_VENC_SLAVE_FIP_MASK;
    }
    switch (slvCfg->fieldDetectionMode)
    {
        case VPSHAL_SDVENC_FD_DET_VSYNC_RISE:
            regOvly->SLAVE |= (VPSHAL_SDVENC_FD_DET_VSYNC_RISE <<
                                (CSL_VPS_SD_VENC_SLAVE_FMD_SHIFT &
                                CSL_VPS_SD_VENC_SLAVE_FMD_MASK));
            break;
        case VPSHAL_SDVENC_FD_DET_EXT_FIELD:
            regOvly->SLAVE |= (VPSHAL_SDVENC_FD_DET_EXT_FIELD <<
                                (CSL_VPS_SD_VENC_SLAVE_FMD_SHIFT &
                                CSL_VPS_SD_VENC_SLAVE_FMD_MASK));
            break;
        case VPSHAL_SDVENC_FD_DET_VSYNC_FID:
            regOvly->SLAVE |= (VPSHAL_SDVENC_FD_DET_VSYNC_FID <<
                                (CSL_VPS_SD_VENC_SLAVE_FMD_SHIFT &
                                CSL_VPS_SD_VENC_SLAVE_FMD_MASK));
        case VPSHAL_SDVENC_FD_DET_VSYNC_PHASE:
            regOvly->SLAVE |= (VPSHAL_SDVENC_FD_DET_VSYNC_PHASE <<
                                (CSL_VPS_SD_VENC_SLAVE_FMD_SHIFT &
                                CSL_VPS_SD_VENC_SLAVE_FMD_MASK));
    }
}

Void sdVencGetCompConfig(vpsHal_SdVencComponentConfig *comCfg,
                         CSL_VpsSdVencRegsOvly regOvly)
{
    comCfg->synctipAmp = (regOvly->CMPNT0 &
                            CSL_VPS_SD_VENC_CMPNT0_MTLVL_MASK) >>
                            CSL_VPS_SD_VENC_CMPNT0_MTLVL_SHIFT;
    comCfg->syncAmp = (regOvly->CMPNT0 &
                            CSL_VPS_SD_VENC_CMPNT0_MSLVL_MASK) >>
                            CSL_VPS_SD_VENC_CMPNT0_MSLVL_SHIFT;

    switch ((regOvly->CMPNT1 & CSL_VPS_SD_VENC_CMPNT1_MPSR_MASK) >>
                CSL_VPS_SD_VENC_CMPNT1_MPSR_SHIFT)
    {
        case VPSHAL_SDVENC_PIC_SYNC_RAT_10_4:
            comCfg->pictSynRatio =  VPSHAL_SDVENC_PIC_SYNC_RAT_10_4;
        break;
        case VPSHAL_SDVENC_PIC_SYNC_RAT_7_3:
            comCfg->pictSynRatio =  VPSHAL_SDVENC_PIC_SYNC_RAT_7_3;
        break;
    }
    switch ((regOvly->CMPNT1 & CSL_VPS_SD_VENC_CMPNT1_MRGB_MASK) >>
                CSL_VPS_SD_VENC_CMPNT1_MRGB_SHIFT)
    {
        case VPSHAL_SDVENC_OUTPUT_RGB:
             comCfg->outColorSpace = VPSHAL_SDVENC_OUTPUT_RGB;
        break;
        case VPSHAL_SDVENC_OUTPUT_YUV:
            comCfg->outColorSpace = VPSHAL_SDVENC_OUTPUT_YUV;
        break;
    }
    comCfg->syncOnYOrG = regOvly->CMPNT1 & CSL_VPS_SD_VENC_CMPNT1_MSOYG_MASK;

    comCfg->syncOnPbOrB = regOvly->CMPNT1 & CSL_VPS_SD_VENC_CMPNT1_MSOUB_MASK;

    comCfg->syncOnPrOrR = regOvly->CMPNT1 & CSL_VPS_SD_VENC_CMPNT1_MSOVR_MASK;

    comCfg->yGUpperLimit = (regOvly->MYGCLP &
                                CSL_VPS_SD_VENC_MYGCLP_MYGUCLP_MASK) >>
                                CSL_VPS_SD_VENC_MYGCLP_MYGUCLP_SHIFT;
    comCfg->yGLowerLimit =  (regOvly->MYGCLP &
                                CSL_VPS_SD_VENC_MYGCLP_MYGLCLP_MASK) >>
                                CSL_VPS_SD_VENC_MYGCLP_MYGLCLP_SHIFT;
    comCfg->uBUpperLimit = (regOvly->MUBCLP &
                                CSL_VPS_SD_VENC_MUBCLP_MUBUCLP_MASK) >>
                                CSL_VPS_SD_VENC_MUBCLP_MUBUCLP_SHIFT;
    comCfg->uBLowerLimit = (regOvly->MUBCLP &
                                CSL_VPS_SD_VENC_MUBCLP_MUBLCLP_MASK) >>
                                CSL_VPS_SD_VENC_MUBCLP_MUBLCLP_SHIFT;
    comCfg->vRUpperLimit = (regOvly->MVRCLP &
                                CSL_VPS_SD_VENC_MVRCLP_MVRUCLP_MASK) >>
                                CSL_VPS_SD_VENC_MVRCLP_MVRUCLP_SHIFT;
    comCfg->vRLowerLimit = (regOvly->MVRCLP &
                                CSL_VPS_SD_VENC_MVRCLP_MVRLCLP_MASK) >>
                                CSL_VPS_SD_VENC_MVRCLP_MVRLCLP_SHIFT;
}

Void sdVencGetCvbsConfig(vpsHal_SdVencCvbsConfig *cvbsCfg,
                         CSL_VpsSdVencRegsOvly regOvly)
{
    cvbsCfg->synctipAmp =  (regOvly->CVBS0 &
                                CSL_VPS_SD_VENC_CVBS0_CTLVL_MASK) >>
                                CSL_VPS_SD_VENC_CVBS0_CTLVL_SHIFT;
    cvbsCfg->syncAmp = (regOvly->CVBS0 &
                                CSL_VPS_SD_VENC_CVBS0_CSLVL_MASK) >>
                                CSL_VPS_SD_VENC_CVBS0_CSLVL_SHIFT;
    regOvly->CVBS1 = 0u;

    switch ((regOvly->CVBS1 & CSL_VPS_SD_VENC_CVBS1_CPSR_MASK) >>
                CSL_VPS_SD_VENC_CVBS1_CPSR_SHIFT)
    {
        case VPSHAL_SDVENC_PIC_SYNC_RAT_10_4:
            cvbsCfg->pictSynRatio = VPSHAL_SDVENC_PIC_SYNC_RAT_10_4;
        break;
        case VPSHAL_SDVENC_PIC_SYNC_RAT_7_3:
            cvbsCfg->pictSynRatio = VPSHAL_SDVENC_PIC_SYNC_RAT_7_3;
        break;
    }
    cvbsCfg->lumaLpfEnable = regOvly->CVBS1 & CSL_VPS_SD_VENC_CVBS1_YLPF_MASK;


    cvbsCfg->chromaLpfEnable = regOvly->CVBS1 & CSL_VPS_SD_VENC_CVBS1_CLPF_MASK;


    switch ((regOvly->CVBS1 & CSL_VPS_SD_VENC_CVBS1_CCM_MASK) >>
                CSL_VPS_SD_VENC_CVBS1_CCM_SHIFT)
    {
        case VPSHAL_SDVENC_COL_MODULATION_MODE_NTSC:
            cvbsCfg->colorModulation = VPSHAL_SDVENC_COL_MODULATION_MODE_NTSC;
        break;
        case VPSHAL_SDVENC_COL_MODULATION_MODE_PAL:
            cvbsCfg->colorModulation = VPSHAL_SDVENC_COL_MODULATION_MODE_PAL;
        break;
        case VPSHAL_SDVENC_COL_MODULATION_MODE_SECAM:
            cvbsCfg->colorModulation = VPSHAL_SDVENC_COL_MODULATION_MODE_SECAM;
        break;
    }

    cvbsCfg->yDelayAdjustment = (regOvly->CVBS1 &
                                    CSL_VPS_SD_VENC_CVBS1_CYDLY_MASK) >>
                                    CSL_VPS_SD_VENC_CVBS1_CYDLY_SHIFT;
    cvbsCfg->burstAmplitude = (regOvly->CVBS1 &
                                    CSL_VPS_SD_VENC_CVBS1_CBLVL_MASK) >>
                                    CSL_VPS_SD_VENC_CVBS1_CBLVL_SHIFT;
    cvbsCfg->yClipUpperLimit = (regOvly->CYGCLP &
                                    CSL_VPS_SD_VENC_CYGCLP_CYUCLP_MASK) >>
                                    CSL_VPS_SD_VENC_CYGCLP_CYUCLP_SHIFT;
    cvbsCfg->yClipLowerLimit = (regOvly->CYGCLP &
                                    CSL_VPS_SD_VENC_CYGCLP_CYLCLP_MASK) >>
                                    CSL_VPS_SD_VENC_CYGCLP_CYLCLP_SHIFT;
    cvbsCfg->uClipUpperLimit = (regOvly->CUBCLP &
                                    CSL_VPS_SD_VENC_CUBCLP_CUUCLP_MASK) >>
                                    CSL_VPS_SD_VENC_CUBCLP_CUUCLP_SHIFT;
    cvbsCfg->uClipLowerLimit = (regOvly->CUBCLP &
                                    CSL_VPS_SD_VENC_CUBCLP_CULCLP_MASK) >>
                                    CSL_VPS_SD_VENC_CUBCLP_CULCLP_SHIFT;
    cvbsCfg->vClipUpperLimit = (regOvly->CVRCLP &
                                    CSL_VPS_SD_VENC_CVRCLP_CVUCLP_MASK) >>
                                    CSL_VPS_SD_VENC_CVRCLP_CVUCLP_SHIFT;
    cvbsCfg->vClipLowerLimit = (regOvly->CVRCLP &
                                    CSL_VPS_SD_VENC_CVRCLP_CVLCLP_MASK) >>
                                    CSL_VPS_SD_VENC_CVRCLP_CVLCLP_SHIFT;
}


Void sdVencGetEtmgConfig(vpsHal_SdVencEtmgConfig *eTmgCfg,
                         CSL_VpsSdVencRegsOvly regOvly)
{
    eTmgCfg->activeVidHorzStartPos = (regOvly->ETMG0 &
                            CSL_VPS_SD_VENC_ETMG0_AV_H_STA_MASK) >>
                            CSL_VPS_SD_VENC_ETMG0_AV_H_STA_SHIFT;
    eTmgCfg->activeVidHorzStopPos = (regOvly->ETMG0 &
                            CSL_VPS_SD_VENC_ETMG0_AV_H_STP_MASK) >>
                            CSL_VPS_SD_VENC_ETMG0_AV_H_STP_SHIFT;
    eTmgCfg->activeVidVertStarPos0 = (regOvly->ETMG1 &
                            CSL_VPS_SD_VENC_ETMG1_AV_V_STA0_MASK) >>
                            CSL_VPS_SD_VENC_ETMG1_AV_V_STA0_SHIFT;
    eTmgCfg->activeVidVertStopPos0 = (regOvly->ETMG1 &
                            CSL_VPS_SD_VENC_ETMG1_AV_V_STP0_MASK) >>
                            CSL_VPS_SD_VENC_ETMG1_AV_V_STP0_SHIFT;
    eTmgCfg->activeVidVertStarPos1 = (regOvly->ETMG2 &
                            CSL_VPS_SD_VENC_ETMG2_AV_V_STA1_MASK) >>
                            CSL_VPS_SD_VENC_ETMG2_AV_V_STA1_SHIFT;
    eTmgCfg->activeVidVertStopPos1 =  (regOvly->ETMG2 &
                            CSL_VPS_SD_VENC_ETMG2_AV_V_STP1_MASK) >>
                            CSL_VPS_SD_VENC_ETMG2_AV_V_STP1_SHIFT;
    eTmgCfg->colorBurstStartPos = (regOvly->ETMG3 &
                            CSL_VPS_SD_VENC_ETMG3_BST_H_STA_MASK) >>
                            CSL_VPS_SD_VENC_ETMG3_BST_H_STA_SHIFT;
    eTmgCfg->colorBurstStopPos = (regOvly->ETMG3 &
                            CSL_VPS_SD_VENC_ETMG3_BST_H_STP_MASK) >>
                            CSL_VPS_SD_VENC_ETMG3_BST_H_STP_SHIFT;
    eTmgCfg->vbiReqStartPos = (regOvly->ETMG4 &
                            CSL_VPS_SD_VENC_ETMG4_VBI_H_STA_MASK) >>
                            CSL_VPS_SD_VENC_ETMG4_VBI_H_STA_SHIFT;
    eTmgCfg->vbiReqStopPos = (regOvly->ETMG4 &
                                CSL_VPS_SD_VENC_ETMG4_VBI_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_ETMG4_VBI_H_STP_SHIFT;
}

Void sdVencGetControlConfig(VpsHal_SdVencConfig *vencCfg,
                            CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->ECTL = 0;

    vencCfg->gammaCorrEnable = regOvly->ECTL & CSL_VPS_SD_VENC_ECTL_GAM_MASK;

    vencCfg->Upsampling2XEnable = regOvly->ECTL & CSL_VPS_SD_VENC_ECTL_UPS_MASK;

    vencCfg->Dac2xOverSamplingEnable = regOvly->ECTL &
                                        CSL_VPS_SD_VENC_ECTL_DUPS_MASK;

    switch ((regOvly->ECTL & CSL_VPS_SD_VENC_ECTL_SVSW_MASK) >>
                CSL_VPS_SD_VENC_ECTL_SVSW_SHIFT)
    {
        case VPSHAL_SDVENC_VERT_SYNC_WID_30:
            vencCfg->vertSyncWidth = VPSHAL_SDVENC_VERT_SYNC_WID_30;
        break;
        case VPSHAL_SDVENC_VERT_SYNC_WID_25:
            vencCfg->vertSyncWidth = VPSHAL_SDVENC_VERT_SYNC_WID_25;
        break;
    }

    vencCfg->blankingShapeEnable = regOvly->ECTL &CSL_VPS_SD_VENC_ECTL_BLS_MASK;

    vencCfg->vbiEnable = regOvly->ECTL & CSL_VPS_SD_VENC_ECTL_VBIEN_MASK;

    switch ((regOvly->ECTL & CSL_VPS_SD_VENC_ECTL_PXLR_MASK) >>
                CSL_VPS_SD_VENC_ECTL_PXLR_SHIFT)
    {
        case VPSHAL_SDVENC_PIX_CLK_1X:
            vencCfg->pixelRateMultiplier = VPSHAL_SDVENC_PIX_CLK_1X;
        break;
        case VPSHAL_SDVENC_PIX_CLK_2X:
            vencCfg->pixelRateMultiplier = VPSHAL_SDVENC_PIX_CLK_2X;
        break;
    }
    switch ((regOvly->ECTL & CSL_VPS_SD_VENC_ECTL_FMT_MASK) >>
                CSL_VPS_SD_VENC_ECTL_FMT_SHIFT)
    {
        case VPSHAL_SDVENC_FMT_525I:
            vencCfg->fmt = VPSHAL_SDVENC_FMT_525I;
        break;
        case VPSHAL_SDVENC_FMT_625I:
            vencCfg->fmt = VPSHAL_SDVENC_FMT_625I;
        break;
        case VPSHAL_SDVENC_FMT_525P:
           vencCfg->fmt = VPSHAL_SDVENC_FMT_525P;
        break;
        case VPSHAL_SDVENC_FMT_625P:
            vencCfg->fmt = VPSHAL_SDVENC_FMT_625P;
        break;
        case VPSHAL_SDVENC_FMT_1080I:
            vencCfg->fmt = VPSHAL_SDVENC_FMT_1080I;
        break;
        case VPSHAL_SDVENC_FMT_720P:
             vencCfg->fmt = VPSHAL_SDVENC_FMT_720P;
        break;
        case VPSHAL_SDVENC_FMT_1080P:
            vencCfg->fmt = VPSHAL_SDVENC_FMT_1080P;
        break;
    }
}

Void sdVencGetDtvConfig(vpsHal_SdVencDtvConfig *dtvCfg,
                       CSL_VpsSdVencRegsOvly regOvly)
{
    dtvCfg->hSyncStartPixel = (regOvly->DTVS0 &
                                CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STA_MASK) >>
                                CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STA_SHIFT;
    dtvCfg->hSyncStopPixel = (regOvly->DTVS0 &
                                CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_DTVS0_DTV_HS_H_STP_SHIFT;

    dtvCfg->vSyncStartPixel = (regOvly->DTVS1 &
                                CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STA_MASK) >>
                                CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STA_SHIFT;
    dtvCfg->vSyncStopPixel = (regOvly->DTVS1 &
                                CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_DTVS1_DTV_VS_H_STP_SHIFT;

    dtvCfg->vSyncStartLine = (regOvly->DTVS2 &
                                CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STA_MASK) >>
                                CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STA_SHIFT;
    dtvCfg->vSyncStopLine = (regOvly->DTVS2 &
                                CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STP_MASK) >>
                                CSL_VPS_SD_VENC_DTVS2_DTV_VS_V_STP_SHIFT;
    dtvCfg->fidTogglePixelPosition = (regOvly->DTVS3 &
                                    CSL_VPS_SD_VENC_DTVS3_DTV_FID_H_STA_MASK) >>
                                    CSL_VPS_SD_VENC_DTVS3_DTV_FID_H_STA_SHIFT;

    dtvCfg->fidStartLine0 = (regOvly->DTVS4 &
                                CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA0_MASK) >>
                                CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA0_SHIFT;
    dtvCfg->fidId0 =  (regOvly->DTVS4 &
                            CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA0_MASK) >>
                            CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA0_SHIFT;
    dtvCfg->fidStartLine1 = (regOvly->DTVS4 &
                                CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA1_MASK) >>
                                CSL_VPS_SD_VENC_DTVS4_DTV_FID_V_STA1_SHIFT;
    dtvCfg->fidId1 = (regOvly->DTVS4 &
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA1_MASK) >>
                        CSL_VPS_SD_VENC_DTVS4_DTV_FID_F_STA1_SHIFT;
    dtvCfg->avidStartPixel = (regOvly->DTVS5 &
                                CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STA_MASK) >>
                                CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STA_SHIFT;
    dtvCfg->avidStopPixel =  (regOvly->DTVS5 &
                                CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_DTVS5_DTV_AVID_H_STP_SHIFT;
    dtvCfg->avidStartLine0 = (regOvly->DTVS6 &
                                CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STA0_MASK) >>
                                CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STA0_SHIFT;
    dtvCfg->avidStopLine0 = (regOvly->DTVS6 &
                                CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STP0_MASK) >>
                                CSL_VPS_SD_VENC_DTVS6_DTV_AVID_V_STP0_SHIFT;

    dtvCfg->avidStartLine1 = (regOvly->DTVS7 &
                                CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STA1_MASK) >>
                                CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STA1_SHIFT;
    dtvCfg->avidStopLine1 = (regOvly->DTVS7 &
                                CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STP1_MASK) >>
                                CSL_VPS_SD_VENC_DTVS7_DTV_AVID_V_STP1_SHIFT;
}

Void sdVencGetDvoConfig(vpsHal_SdVencDvoConfig *dvoCfg,
                       CSL_VpsSdVencRegsOvly regOvly)
{
    dvoCfg->hSyncStartPixel =  (regOvly->DVOS0 &
                                    CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STA_MASK) >>
                                    CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STA_SHIFT;
    dvoCfg->hSyncStopPixel = (regOvly->DVOS0 &
                                CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_DVOS0_DVO_HS_H_STP_SHIFT;
    dvoCfg->vSyncStartPixel = (regOvly->DVOS1 &
                                    CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STA_MASK) >>
                                    CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STA_SHIFT;
    dvoCfg->vSyncStopPixel = (regOvly->DVOS1 &
                                    CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STP_MASK) >>
                                    CSL_VPS_SD_VENC_DVOS1_DVO_VS_H_STP_SHIFT;
    dvoCfg->vSyncStartLine  = (regOvly->DVOS2 &
                                    CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STA_MASK) >>
                                    CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STA_SHIFT;
    dvoCfg->vSyncStopLine = (regOvly->DVOS2 &
                                CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STP_MASK) >>
                                CSL_VPS_SD_VENC_DVOS2_DVO_VS_V_STP_SHIFT;
    dvoCfg->fidTogglePixelPosition = (regOvly->DVOS3 &
                                    CSL_VPS_SD_VENC_DVOS3_DVO_FID_H_STA_MASK) >>
                                    CSL_VPS_SD_VENC_DVOS3_DVO_FID_H_STA_SHIFT;
    dvoCfg->horzDelay = (regOvly->DVOS3 &
                            CSL_VPS_SD_VENC_DVOS3_DVO_DELAY_MASK) >>
                            CSL_VPS_SD_VENC_DVOS3_DVO_DELAY_SHIFT;
    dvoCfg->fidStartLine0 = (regOvly->DVOS4 &
                                CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA0_MASK) >>
                                CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA0_SHIFT;
    dvoCfg->fidId0 = (regOvly->DVOS4 &
                            CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA0_MASK) >>
                            CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA0_SHIFT;
    dvoCfg->fidStartLine1 =  (regOvly->DVOS4 &
                                CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA1_MASK) >>
                                CSL_VPS_SD_VENC_DVOS4_DVO_FID_V_STA1_SHIFT;
    dvoCfg->fidId1 = (regOvly->DVOS4 &
                                CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA1_MASK) >>
                                CSL_VPS_SD_VENC_DVOS4_DVO_FID_F_STA1_SHIFT;
    dvoCfg->avidStartPixel = (regOvly->DVOS5 &
                                CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STA_MASK) >>
                                CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STA_SHIFT;
    dvoCfg->avidStopPixel =  (regOvly->DVOS5 &
                                CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_DVOS5_DVO_AVID_H_STP_SHIFT;
    dvoCfg->avidStartLine0 = (regOvly->DVOS6 &
                                CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STA0_MASK) >>
                                CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STA0_SHIFT;
    dvoCfg->avidStopLine0 = (regOvly->DVOS6 &
                                CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STP0_MASK) >>
                                CSL_VPS_SD_VENC_DVOS6_DVO_AVID_V_STP0_SHIFT;
    dvoCfg->avidStartLine1 = (regOvly->DVOS7 &
                                CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STA1_MASK) >>
                                CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STA1_SHIFT;
    dvoCfg->avidStopLine1 = (regOvly->DVOS7 &
                                CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STP1_MASK) >>
                                CSL_VPS_SD_VENC_DVOS7_DVO_AVID_V_STP1_SHIFT;
}

Void sdVencGetTvDetectConfig(VpsHal_SdVencTvDetConfig *tvDetCfg,
                          CSL_VpsSdVencRegsOvly regOvly)
{
    tvDetCfg->tvDeteStartPixel = (regOvly->TVDETGP0 &
                                CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STA_MASK) >>
                                CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STA_SHIFT;
    tvDetCfg->tvDeteStopPixel = (regOvly->TVDETGP0 &
                                CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STP_MASK) >>
                                CSL_VPS_SD_VENC_TVDETGP0_TVDETGP_H_STP_SHIFT;
    tvDetCfg->tvDeteStartLine = (regOvly->TVDETGP1 &
                                CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STA_MASK) >>
                                CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STA_SHIFT;
    tvDetCfg->teDeteStopLine = (regOvly->TVDETGP1 &
                                CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STP_MASK) >>
                                CSL_VPS_SD_VENC_TVDETGP1_TVDETGP_V_STP_SHIFT;
}

Void sdVencGetPolarityConfig(vpsHal_SdVencDtvConfig *dtvCfg,
                             vpsHal_SdVencDvoConfig *dvoCfg,
                             VpsHal_SdVencTvDetConfig *tvDetCfg,
                             CSL_VpsSdVencRegsOvly regOvly)
{
    regOvly->POL = 0;

    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DTV_HS_POL_MASK)
    {
        dtvCfg->hsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dtvCfg->hsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DTV_VS_POL_MASK)
    {
        dtvCfg->vsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dtvCfg->vsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DTV_FID_POL_MASK)
    {
        dtvCfg->fidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dtvCfg->fidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DTV_AVID_POL_MASK)
    {
        dtvCfg->activVidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dtvCfg->activVidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DVO_HS_POL_MASK)
    {
        dvoCfg->hsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dvoCfg->hsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DVO_VS_POL_MASK)
    {
        dvoCfg->vsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dvoCfg->vsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DVO_FID_POL_MASK)
    {
        dvoCfg->fidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dvoCfg->fidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_DVO_AVID_POL_MASK)
    {
        dvoCfg->activVidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        dvoCfg->activVidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == regOvly->POL & CSL_VPS_SD_VENC_POL_TVDETGP_POL_MASK)
    {
        tvDetCfg->tvDetePolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        tvDetCfg->tvDetePolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
}

Void sdVencGetSlaveCfg(vpsHal_SdVencSalveConfig *slvCfg,
                        CSL_VpsSdVencRegsOvly    regOvly)
{
    if (TRUE == (regOvly->SLAVE & CSL_VPS_SD_VENC_SLAVE_HIP_MASK))
    {
        slvCfg->hsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        slvCfg->hsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == (regOvly->SLAVE & CSL_VPS_SD_VENC_SLAVE_VIP_MASK))
    {
        slvCfg->vsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        slvCfg->vsPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    if (TRUE == (regOvly->SLAVE & CSL_VPS_SD_VENC_SLAVE_FIP_MASK))
    {
        slvCfg->fidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_LOW;
    }
    else
    {
        slvCfg->fidPolarity = VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH;
    }
    switch ((regOvly->SLAVE & CSL_VPS_SD_VENC_SLAVE_FMD_MASK) >>
                CSL_VPS_SD_VENC_SLAVE_FMD_SHIFT)
    {
        case VPSHAL_SDVENC_FD_DET_VSYNC_RISE:
            slvCfg->fieldDetectionMode = VPSHAL_SDVENC_FD_DET_VSYNC_RISE;
            break;
        case VPSHAL_SDVENC_FD_DET_EXT_FIELD:
            slvCfg->fieldDetectionMode = VPSHAL_SDVENC_FD_DET_EXT_FIELD;
            break;
        case VPSHAL_SDVENC_FD_DET_VSYNC_FID:
            slvCfg->fieldDetectionMode = VPSHAL_SDVENC_FD_DET_VSYNC_FID;
            break;
        case VPSHAL_SDVENC_FD_DET_VSYNC_PHASE:
            slvCfg->fieldDetectionMode = VPSHAL_SDVENC_FD_DET_VSYNC_PHASE;
            break;
    }
}

static Int32 VpsHal_sdVencSetControl(SdVenc_Obj *sdVencObj, Vps_DcVencControl *ctrl)
{
    Int32 retVal = VPS_SOK;
    Int32 temp, tempHue, tempMul;
    UInt32 cnt1, cnt2;
    VpsHal_SdVencCscCoeff *coeff;

    if (VPS_DC_CTRL_HUE == ctrl->control)
    {
        if ((ctrl->level < 1) || (ctrl->level > 61))
        {
            GT_0trace(VpsHalTrace, GT_ERR, "level out of range!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }
    else
    {
        if ((ctrl->level < 1) || (ctrl->level > 256))
        {
            GT_0trace(VpsHalTrace, GT_ERR, "level out of range!!\n");
            retVal = VPS_EOUT_OF_RANGE;
        }
    }
    if (VPS_DC_CTRL_BRIGHTNESS == ctrl->control)
    {
        sdVencObj->brightness = ctrl->level - 129;
    }
    else if (VPS_DC_CTRL_CONTRAST == ctrl->control)
    {
        sdVencObj->contrast = ctrl->level;
    }
    else if (VPS_DC_CTRL_HUE == ctrl->control)
    {
        sdVencObj->hue = ctrl->level;
    }
    else
    {
        sdVencObj->saturation = ctrl->level;
    }

    if (FVID2_SOK == retVal)
    {
        if (FVID2_STD_NTSC == sdVencObj->vencCfg.sdVencMode)
        {
            coeff = &defaultCvbsCscCoeffs_NTSC;
        }
        else
        {
            coeff = &defaultCvbsCscCoeffs_PAL;
        }

        tempHue = sdVencObj->hue - 31;

        tempMul = 1;
        if (tempHue < 0)
        {
            tempMul = (-1);
            tempHue = tempHue * tempMul;
        }
        for (cnt1 = 0u; cnt1 < 3;  cnt1 ++)
        {
            for (cnt2 = 0u; cnt2 < 3; cnt2 ++)
            {
                temp = coeff->mulCoeff[cnt1][cnt2] * sdVencObj->contrast / 128;

                if (cnt1 > 0)
                {
                    temp = temp * sdVencObj->saturation / 128;
                }
                if (1u == cnt1)
                {
                    temp = (temp * VpsHalSdVencCosineTable[tempHue] +
                                temp * VpsHalSdVencSineTable[tempHue]) *
                                    tempMul;
                    temp /= 10000;
                }
                if (2u == cnt1)
                {
                    temp = (temp * VpsHalSdVencCosineTable[tempHue] -
                                temp * VpsHalSdVencSineTable[tempHue]) *
                                    tempMul;
                    temp /= 10000;
                }
                sdVencObj->cscCoeff.mulCoeff[cnt1][cnt2] = (UInt16) temp;
            }
        }

        sdVencObj->cscCoeff.outOff[0] = (UInt16) (coeff->outOff[0] + sdVencObj->brightness);
        sdVencObj->cscCoeff.outOff[1] = coeff->outOff[1];
        sdVencObj->cscCoeff.outOff[2] = coeff->outOff[2];


        sdVencSetCvbsCscCoeffs(sdVencObj, &sdVencObj->cscCoeff);
    }

    return (retVal);
}

static Int32 VpsHal_sdVencGetControl(SdVenc_Obj *sdVencObj, Vps_DcVencControl *ctrl)
{
    Int32 retVal = VPS_SOK;

    GT_assert(VpsHalTrace, (NULL != sdVencObj));
    GT_assert(VpsHalTrace, (NULL != ctrl));

    if (VPS_DC_CTRL_BRIGHTNESS == ctrl->control)
    {
        ctrl->level = sdVencObj->brightness + 129;
    }
    else if (VPS_DC_CTRL_CONTRAST == ctrl->control)
    {
        ctrl->level = sdVencObj->contrast;
    }
    else if (VPS_DC_CTRL_HUE == ctrl->control)
    {
        ctrl->level = sdVencObj->hue;
    }
    else if (VPS_DC_CTRL_SATURATION == ctrl->control)
    {
        ctrl->level = sdVencObj->saturation;
    }
    else
    {
        retVal = VPS_EINVALID_PARAMS;
    }

    return (retVal);
}

