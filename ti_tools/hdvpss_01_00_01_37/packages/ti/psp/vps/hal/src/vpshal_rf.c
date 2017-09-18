/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_rf.c
 *
 * \brief VPS RF Modulator Source file.
 * This file implements the HAL APIs of the VPS RF Modulator.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/vps_cfgRf.h>
#include <ti/psp/vps/vps_displayCtrl.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>

#include <ti/psp/vps/hal/vpshal_rf.h>

#include <ti/psp/cslr/cslr_vps_rf_mod.h>

#include <ti/psp/vps/hal/src/vpshalRfDefaults.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* TBD Move this to VPS_CONFIG.H */
#define rfTrace         (VpsHalRfTrace)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief struct Rf_Info
 *  Structure containing Instance Information for each RF module
 *
 *  parent - Pointer to the main scalar info structure  //tbd
 */
typedef struct
{
    UInt32                  instId;
    /**< Indicates the path on which RF is available */

    UInt32                  openCnt;
    /**< Keeps track of number of open for a RF instance */

    CSL_VpsRfModRegsOvly    regOvly;
    /**< Pointer to the CSLR register overlay structure */

    UInt32                  pairedVencId;
    /**< DC assigned identifier of VENC, that is paired with this instance
         of RF */

    UInt32                  encInstEd;
    /**< DC assigned identifier for this instance of encoder, i.e. RF instaces
         identifier as assigned by DC */
} Rf_Info;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void VpsHal_rfPrintExpertConfig(const VpsHal_RfExpertConfig *expConfig);
static Void VpsHal_rfSetConfig(VpsHal_Handle handle,
                        const Vps_RfConfig *config);
static Void VpsHal_rfGetConfig(VpsHal_Handle handle, Vps_RfConfig *config);
static Void VpsHal_rfSetExpertConfig(Rf_Info *rfInstInfo,
                              const VpsHal_RfExpertConfig *expConfig);
static Void VpsHal_rfGetExpertConfig(Rf_Info *rfInstInfo,
                              VpsHal_RfExpertConfig *expConfig);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief RF objects - Module variable to store information about each RF
 *  instance.
 *  Note: If the number of RF instance increases, then this variable should be
 *  initialized with the added RF instance information.
 */
static Rf_Info RfObj[VPSHAL_RF_MAX_INST];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Int VpsHal_rfInit(UInt32 numInstances,
                   const VpsHal_RfInstParams *initParams,
                   Ptr arg)
{
    Int                     instCnt;
    Int                     ret = 0;
    Rf_Info                 *instInfo = NULL;
    VpsHal_RfExpertConfig   rfExpertCfg = VPSHAL_RF_DEFAULT_EXPERT_VAL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfInit");

    /* Check for errors */
    GT_assert(VpsHalTrace, (numInstances <= VPSHAL_RF_MAX_INST));
    GT_assert(VpsHalTrace, (NULL != initParams));

    /* Set the default Values for non-changing parameters */
    for (instCnt = 0u; instCnt < numInstances; instCnt++)
    {
        GT_assert(VpsHalTrace,
                    (initParams[instCnt].instId < VPSHAL_RF_MAX_INST));

        /* Initialize Instance Specific Parameters */
        instInfo = &(RfObj[instCnt]);
        instInfo->openCnt = 0u;
        instInfo->regOvly =
                (CSL_VpsRfModRegsOvly) initParams[instCnt].baseAddress;
        instInfo->instId = initParams[instCnt].instId;
        instInfo->pairedVencId = initParams[instCnt].vencId;
        instInfo->encInstEd = initParams[instCnt].encoderId;

        VpsHal_rfSetExpertConfig(instInfo, &rfExpertCfg);
    }

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfInit<<<<");

    return (ret);
}


Int VpsHal_rfDeInit(Ptr arg)
{
    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfDeInit");
    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfDeInit<<<<");

    return (0);
}


VpsHal_Handle VpsHal_rfOpen(UInt32 rfInst)
{
    Int                 cnt;
    UInt32              cookie;
    VpsHal_Handle       handle = NULL;
    Rf_Info             *instInfo = NULL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfOpen");

    for (cnt = 0; cnt < VPSHAL_RF_MAX_INST; cnt++)
    {
        instInfo = &(RfObj[cnt]);

        /* Return the matching instance handle */
        if (rfInst == instInfo->instId)
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

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfOpen<<<<");

    return (handle);
}


Int VpsHal_rfClose(VpsHal_Handle handle)
{
    Int         ret = -1;
    UInt32      cookie;
    Rf_Info     *instInfo = NULL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfClose");

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    instInfo = (Rf_Info *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (instInfo->openCnt)
    {
        instInfo->openCnt--;
        ret = 0;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfClose<<<<");

    return (ret);
}


Int32 VpsHal_rfStart(VpsHal_Handle handle,
                    Ptr args)
{
    CSL_VpsRfModRegsOvly    regOvly = NULL;
    UInt32                  tempReg = 0x0u;
    Rf_Info                 *instInfo = NULL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfStart");

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Get the register Overlay pointer */
    instInfo = (Rf_Info *) handle;
    regOvly = instInfo->regOvly;

    /* Read the control register */
    tempReg = regOvly->CTRL_REG;

    /* Set dac_on_off bit */
    tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK;

    /* Write the control register */
    regOvly->CTRL_REG = tempReg;

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfStart<<<<");

    return (0);
}


Int32 VpsHal_rfStop (VpsHal_Handle handle,
                    Ptr args)
{
    CSL_VpsRfModRegsOvly    regOvly = NULL;
    UInt32                  tempReg = 0x0u;
    Rf_Info                 *instInfo = NULL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfStart");

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));

    /* Get the register Overlay pointer */
    instInfo = (Rf_Info *) handle;
    regOvly = instInfo->regOvly;

    /* Read the control register */
    tempReg = regOvly->CTRL_REG;

    /* Reset dac_on_off bit */
    tempReg &= ~CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK;

    /* Write the control register */
    regOvly->CTRL_REG = tempReg;

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfStop<<<<");

    return (0);
}


Int32 VpsHal_rfControl(VpsHal_Handle handle,
                        UInt32 cmd,
                        Ptr cmdArgs,
                        Ptr additionalArgs)
{
    Int32                   rtnValue    =   VPS_SOK;
    Vps_DcOnchipEncoderCmd  *encCmd     =   NULL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfControl");

    while(TRUE)
    {
        /* Validate the handle and execute the command. */
        if ((handle == NULL) || (cmdArgs == NULL))
        {
            rtnValue = VPS_EBADARGS;
            GT_0trace(rfTrace, GT_ERR, "Invalid handle/cmdArgs pointer");
            break;
        }

        encCmd = (Vps_DcOnchipEncoderCmd *)cmdArgs;
        if (encCmd->encoderId != VPS_DC_ENCODER_RF)
        {
            rtnValue = VPS_EBADARGS;
            GT_0trace(rfTrace, GT_ERR, "Encoder is not RF");
            break;
        }

        switch(cmd)
        {
            case IOCTL_VPS_DCTRL_RF_START:
            {
                rtnValue = VpsHal_rfStart(handle, NULL);
                break;
            }

            case IOCTL_VPS_DCTRL_RF_STOP:
            {
                rtnValue = VpsHal_rfStop(handle, NULL);
                break;
            }

            case IOCTL_VPS_DCTRL_RF_GET_CONFIG:
            {
                VpsHal_rfGetConfig(handle, (Vps_RfConfig *) encCmd->argument);
                break;
            }

            case IOCTL_VPS_DCTRL_RF_SET_CONFIG:
            {
                VpsHal_rfSetConfig(handle, (Vps_RfConfig *) encCmd->argument);
                break;
            }

            case IOCTL_VPS_DCTRL_RF_GET_EXPERT_CONFIG:
            {
                VpsHal_rfGetExpertConfig(handle,
                                    (VpsHal_RfExpertConfig *) encCmd->argument);
                break;
            }

            case IOCTL_VPS_DCTRL_RF_SET_EXPERT_CONFIG:
            {
                VpsHal_rfSetExpertConfig(handle,
                                    (VpsHal_RfExpertConfig *) encCmd->argument);
                break;
            }

            default :
            {
                rtnValue = VPS_EBADARGS;
                GT_0trace(rfTrace, GT_ERR, "Un-recoganized command");
                break;
            }
        }
        break;
    }

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfControl<<<<");

    return(rtnValue);
}


static void VpsHal_rfSetConfig(VpsHal_Handle handle,
                        const Vps_RfConfig *config)
{
    CSL_VpsRfModRegsOvly    regOvly = NULL;
    UInt32                  tempReg = 0x0u;
    Rf_Info                 *instInfo = NULL;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfSetConfig");

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Get the register Overlay pointer */
    instInfo = (Rf_Info *) handle;
    regOvly = instInfo->regOvly;

    /* Read the control register */
    tempReg = regOvly->CTRL_REG;

    /* Set/reset ch3_4 bit */
    if (TRUE == config->channel34Select)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_CH3_4_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_CTRL_REG_CH3_4_MASK;
    }

    /* Set/reset dac_rst_pol bit */
    if (TRUE == config->dacResetPolarity)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_RST_POL_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_CTRL_REG_DAC_RST_POL_MASK;
    }

    /* Set/reset dac_on_off bit */
    if (TRUE == config->dacOnOff)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK;
    }

    /* Set/reset dac_data_zero bit */
    if (TRUE == config->dacDataZero)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_ZERO_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_ZERO_MASK;
    }

    /* Set/reset dac_data_invert bit */
    if (TRUE == config->dacDataInvert)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_INVERT_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_INVERT_MASK;
    }

    /* Write the control register */
    regOvly->CTRL_REG = tempReg;

    //TBD - Do a reset ???

    /* Read Audio Volume and Mute Control Reg */
    tempReg = regOvly->VOL_REG;

    /* Set the volume */
    tempReg |= (config->audioInVol & CSL_VPS_RF_MOD_VOL_REG_VOLUME_MASK);

    /* Set/reset mute */
    if (TRUE == config->audioMute)
    {
        tempReg |= CSL_VPS_RF_MOD_VOL_REG_MUTE_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_VOL_REG_MUTE_MASK;
    }

    /* Set mono/stereo */
    if (TRUE == config->audioIsMono)
    {
        tempReg |= CSL_VPS_RF_MOD_VOL_REG_MONO_MASK;
    }
    else
    {
        tempReg &= ~CSL_VPS_RF_MOD_VOL_REG_MONO_MASK;
    }

    /* Write the volume register */
    regOvly->VOL_REG = tempReg;

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfSetConfig<<<<");
}


static void VpsHal_rfGetConfig(VpsHal_Handle handle, Vps_RfConfig *config)
{
    CSL_VpsRfModRegsOvly    regOvly = NULL;
    UInt32                  tempReg = 0x0u;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfGetConfig");

    /* Check for NULL pointer */
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = ((Rf_Info *) handle)->regOvly;

    /* Initialize config structure to zero */
    VpsUtils_memset(config, 0, sizeof(Vps_RfConfig));

    /* Read the control register */
    tempReg = regOvly->CTRL_REG;

    /* Read ch3_4 bit */
    if (0 != tempReg & CSL_VPS_RF_MOD_CTRL_REG_CH3_4_MASK)
    {
        config->channel34Select = TRUE;
    }

    /* Read dac_rst_pol bit */
    if (0 != tempReg & CSL_VPS_RF_MOD_CTRL_REG_DAC_RST_POL_MASK)
    {
        config->dacResetPolarity = TRUE;
    }

    /* Read dac_on_off bit */
    if (0 != tempReg & CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK)
    {
        config->dacOnOff = TRUE;
    }

    /* Read dac_data_zero bit */
    if (0 != tempReg & CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_ZERO_MASK)
    {
        config->dacDataZero = TRUE;
    }

    /* Read dac_data_invert bit */
    if (0 != tempReg & CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_INVERT_MASK)
    {
        config->dacDataInvert = TRUE;
    }

    /* Read the volume register */
    tempReg = regOvly->VOL_REG;

    /* Read the volume */
    config->audioInVol  = tempReg & CSL_VPS_RF_MOD_VOL_REG_VOLUME_MASK;

    /* Read mute */
    if (0 != tempReg & CSL_VPS_RF_MOD_VOL_REG_MUTE_MASK)
    {
        config->audioMute = TRUE;
    }

    /* Read mono/stereo */
    if (0 != tempReg & CSL_VPS_RF_MOD_VOL_REG_MONO_MASK)
    {
        config->audioIsMono = TRUE;
    }

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfGetConfig<<<<");
}


static void VpsHal_rfSetExpertConfig(Rf_Info *rfInstInfo,
                              const VpsHal_RfExpertConfig *expConfig)
{
    CSL_VpsRfModRegsOvly    regOvly = NULL;
    UInt32                  tempReg = 0x0u;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfSetExpertConfig");

    GT_assert(VpsHalTrace, (NULL != rfInstInfo));
    GT_assert(VpsHalTrace, (NULL != expConfig));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = rfInstInfo->regOvly;

    /* Copy Video LPF coefficients */
    VpsUtils_memcpy((Ptr)&regOvly->VLPF_REG[0], expConfig->rfCoeff.videoLpfCoeff,
                    sizeof(expConfig->rfCoeff.videoLpfCoeff));

    /* Copy Video GDP coefficients */
    VpsUtils_memcpy((Ptr)&regOvly->VGDP_REG[0], expConfig->rfCoeff.videoGdpCoeff,
                    sizeof(expConfig->rfCoeff.videoGdpCoeff));

    /* Program other registers */
    regOvly->WSN_REG0 = expConfig->wbSpecNormal0;
    regOvly->WSN_REG1 = expConfig->wbSpecNormal1;
    regOvly->MSMA_REG0 = expConfig->audioMainMaxAmp;
    regOvly->MSMA_REG1 = expConfig->audioStereoMaxAmp;
    regOvly->MSFMG_REG0 = expConfig->audioMainFmGain;
    regOvly->MSFMG_REG1 = expConfig->audioStereoFmGain;
    regOvly->MSVD_REG0 = expConfig->audioMainVarDelay;
    regOvly->MSVD_REG1 = expConfig->audioStereoVarDelay;
    regOvly->SOL_REG = expConfig->audioStereoOvermodLimit;

    /* Set the volume and mute control register */
    tempReg = expConfig->audioInVol & CSL_VPS_RF_MOD_VOL_REG_VOLUME_MASK;
    if (TRUE == expConfig->audioMute)
    {
        tempReg |= CSL_VPS_RF_MOD_VOL_REG_MUTE_MASK;
    }
    if (TRUE == expConfig->audioIsMono)
    {
        tempReg |= CSL_VPS_RF_MOD_VOL_REG_MONO_MASK;
    }
    regOvly->VOL_REG = tempReg;

    /* Set the control and channel 3/4 select register */
    tempReg = 0x0u;
    if (TRUE == expConfig->channel34Select)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_CH3_4_MASK;
    }
    if (TRUE == expConfig->dacResetPolarity)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_RST_POL_MASK;
    }
    if (TRUE == expConfig->dacOnOff)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK;
    }
    if (TRUE == expConfig->dacDataZero)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_ZERO_MASK;
    }
    if (TRUE == expConfig->dacDataInvert)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_INVERT_MASK;
    }
    regOvly->CTRL_REG = tempReg;

    regOvly->VRFAMP_REG = expConfig->videoRfAmp;
    regOvly->VRFGAIN_REG = expConfig->videoRfGain;
    regOvly->VSTLSB_REG = expConfig->videoSyncTip;
    regOvly->ABLC_REG = expConfig->audioBbLvlCtrl;
    regOvly->AFMAMP_REG = expConfig->audioFmAmpl;
    regOvly->MAT_REG = expConfig->audioMainAmpThld;
    regOvly->SAMAMPTHRES_REG = expConfig->audioAmAmpThld;
    regOvly->MPLUSSAMPTHRES_REG = expConfig->audioMainPlusThld;
    regOvly->MPLUSSMAXAMP_REG = expConfig->audioMainPlusMaxAmp;
    regOvly->AGCLIMITTHRES_REG = expConfig->audioAgcLimThld;
    regOvly->AGCGAINDECINC_REG = expConfig->audioAgcDecrIncr;
    regOvly->AGCMINGAINLIMIT_REG = expConfig->audioAgcMinGain;

    regOvly->JITFLT_OFFSET_REG = expConfig->jitterFilterOff;
    regOvly->JITFLT_COEF_REG = expConfig->jitterFilterCoeff;
    regOvly->JITFLT_PATH_ENABLE_REG = expConfig->jitterFilterEn;

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfSetExpertConfig<<<<");
}


static void VpsHal_rfGetExpertConfig(Rf_Info *rfInstInfo,
                              VpsHal_RfExpertConfig *expConfig)
{
    CSL_VpsRfModRegsOvly    regOvly = NULL;
    UInt32                  tempReg = 0x0u;

    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfGetExpertConfig");

    GT_assert(VpsHalTrace, (NULL != rfInstInfo));
    GT_assert(VpsHalTrace, (NULL != expConfig));

    /* Get the Register Overlay Pointer from the handle */
    regOvly = rfInstInfo->regOvly;

    /* Initialize expert config structure to zero */
    VpsUtils_memset(expConfig, 0, sizeof(VpsHal_RfExpertConfig));

    /* Copy Video LPF coefficients */
    VpsUtils_memcpy(expConfig->rfCoeff.videoLpfCoeff, (Ptr)&regOvly->VLPF_REG[0],
                    sizeof(expConfig->rfCoeff.videoLpfCoeff));

    /* Copy Video GDP coefficients */
    VpsUtils_memcpy(expConfig->rfCoeff.videoGdpCoeff, (Ptr)&regOvly->VGDP_REG[0],
                    sizeof(expConfig->rfCoeff.videoGdpCoeff));

    /* Read other registers */
    expConfig->wbSpecNormal0 = regOvly->WSN_REG0 &
                                CSL_VPS_RF_MOD_WSN_REG0_WSN0_MASK;
    expConfig->wbSpecNormal1 = regOvly->WSN_REG1 &
                                CSL_VPS_RF_MOD_WSN_REG1_WSN1_MASK;
    expConfig->audioMainMaxAmp = regOvly->MSMA_REG0 &
                                CSL_VPS_RF_MOD_MSMA_REG0_MS_MA0_MASK;
    expConfig->audioStereoMaxAmp = regOvly->MSMA_REG1 &
                                CSL_VPS_RF_MOD_MSMA_REG1_MS_MA1_MASK;
    expConfig->audioMainFmGain = regOvly->MSFMG_REG0 &
                                CSL_VPS_RF_MOD_MSFMG_REG0_MS_FM_G0_MASK;
    expConfig->audioStereoFmGain = regOvly->MSFMG_REG1 &
                                CSL_VPS_RF_MOD_MSFMG_REG1_MS_FM_G1_MASK;
    expConfig->audioMainVarDelay = regOvly->MSVD_REG0 &
                                CSL_VPS_RF_MOD_MSVD_REG0_MS_VD0_MASK;
    expConfig->audioStereoVarDelay = regOvly->MSVD_REG1 &
                                CSL_VPS_RF_MOD_MSVD_REG1_MS_VD1_MASK;
    expConfig->audioStereoOvermodLimit = regOvly->SOL_REG &
                                CSL_VPS_RF_MOD_SOL_REG_SOL_MASK;

    /* Set the volume and mute control register */
    tempReg = expConfig->audioInVol & CSL_VPS_RF_MOD_VOL_REG_VOLUME_MASK;
    if (TRUE == expConfig->audioMute)
    {
        tempReg |= CSL_VPS_RF_MOD_VOL_REG_MUTE_MASK;
    }
    if (TRUE == expConfig->audioIsMono)
    {
        tempReg |= CSL_VPS_RF_MOD_VOL_REG_MONO_MASK;
    }
    regOvly->VOL_REG = tempReg;

    /* Set the control and channel 3/4 select register */
    tempReg = 0x0u;
    if (TRUE == expConfig->channel34Select)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_CH3_4_MASK;
    }
    if (TRUE == expConfig->dacResetPolarity)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_RST_POL_MASK;
    }
    if (TRUE == expConfig->dacOnOff)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_ON_OFF_MASK;
    }
    if (TRUE == expConfig->dacDataZero)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_ZERO_MASK;
    }
    if (TRUE == expConfig->dacDataInvert)
    {
        tempReg |= CSL_VPS_RF_MOD_CTRL_REG_DAC_DATA_INVERT_MASK;
    }
    regOvly->CTRL_REG = tempReg;

    expConfig->videoRfAmp = regOvly->VRFAMP_REG &
                            CSL_VPS_RF_MOD_VRFAMP_REG_VRF_AMP_MASK;
    expConfig->videoRfGain = regOvly->VRFGAIN_REG &
                            CSL_VPS_RF_MOD_VRFGAIN_REG_VRF_GAIN_MASK;
    expConfig->videoSyncTip = regOvly->VSTLSB_REG &
                            CSL_VPS_RF_MOD_VSTLSB_REG_VSTLSB_MASK;
    expConfig->audioBbLvlCtrl = regOvly->ABLC_REG &
                            CSL_VPS_RF_MOD_ABLC_REG_ABLC_MASK;
    expConfig->audioFmAmpl = regOvly->AFMAMP_REG &
                            CSL_VPS_RF_MOD_AFMAMP_REG_AFMAMP_MASK;
    expConfig->audioMainAmpThld = regOvly->MAT_REG &
                            CSL_VPS_RF_MOD_MAT_REG_MAT_MASK;
    expConfig->audioAmAmpThld = regOvly->SAMAMPTHRES_REG &
                            CSL_VPS_RF_MOD_SAMAMPTHRES_REG_SAMAMPTHRES_MASK;
    expConfig->audioMainPlusThld = regOvly->MPLUSSAMPTHRES_REG &
                        CSL_VPS_RF_MOD_MPLUSSAMPTHRES_REG_MPLUSSAMPTHRES_MASK;
    expConfig->audioMainPlusMaxAmp = regOvly->MPLUSSMAXAMP_REG &
                            CSL_VPS_RF_MOD_MPLUSSMAXAMP_REG_MPLUSSMAXAMP_MASK;
    expConfig->audioAgcLimThld = regOvly->AGCLIMITTHRES_REG &
                            CSL_VPS_RF_MOD_AGCLIMITTHRES_REG_AGCLIMITTHRES_MASK;
    expConfig->audioAgcDecrIncr = regOvly->AGCGAINDECINC_REG &
                            CSL_VPS_RF_MOD_AGCGAINDECINC_REG_AGCGAINDECINC_MASK;
    expConfig->audioAgcMinGain = regOvly->AGCMINGAINLIMIT_REG &
                        CSL_VPS_RF_MOD_AGCMINGAINLIMIT_REG_AGCMINGAINLIMIT_MASK;

    expConfig->jitterFilterOff = regOvly->JITFLT_OFFSET_REG &
                            CSL_VPS_RF_MOD_JITFLTOFFSET_REG_JITFLTOFFSET_MASK;
    expConfig->jitterFilterCoeff = regOvly->JITFLT_COEF_REG &
                            CSL_VPS_RF_MOD_JITFLTCOEF_REG_JITFLTCOEF_MASK;
    expConfig->jitterFilterEn = regOvly->JITFLT_PATH_ENABLE_REG &
                    CSL_VPS_RF_MOD_JITFLTPATHENABLE_REG_JITFLTPATHENABLE_MASK;

    VpsHal_rfPrintExpertConfig(expConfig);

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfGetExpertConfig<<<<");
}


static Void VpsHal_rfPrintExpertConfig(const VpsHal_RfExpertConfig *expConfig)
{
    GT_0trace(rfTrace, GT_ENTER, ">>>>VpsHal_rfPrintExpertConfig");

    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".wbSpecNormal0             = %d,\n", expConfig->wbSpecNormal0);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".wbSpecNormal1             = %d,\n", expConfig->wbSpecNormal1);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMainMaxAmp           = %d,\n", expConfig->audioMainMaxAmp);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioStereoMaxAmp         = %d,\n", expConfig->audioStereoMaxAmp);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMainFmGain           = %d,\n", expConfig->audioMainFmGain);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioStereoFmGain         = %d,\n", expConfig->audioStereoFmGain);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMainVarDelay         = %d,\n", expConfig->audioMainVarDelay);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioStereoVarDelay       = %d,\n", expConfig->audioStereoVarDelay);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioStereoOvermodLimit   = %d,\n", expConfig->audioStereoOvermodLimit);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioInVol                = %d,\n", expConfig->audioInVol);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMute                 = %d,\n", expConfig->audioMute);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioIsMono               = %d,\n", expConfig->audioIsMono);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".channel34Select           = %d,\n", expConfig->channel34Select);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dacResetPolarity          = %d,\n", expConfig->dacResetPolarity);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dacOnOff                  = %d,\n", expConfig->dacOnOff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dacDataZero               = %d,\n", expConfig->dacDataZero);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".dacDataInvert             = %d,\n", expConfig->dacDataInvert);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".videoRfAmp                = %d,\n", expConfig->videoRfAmp);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".videoRfGain               = %d,\n", expConfig->videoRfGain);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".videoSyncTip              = %d,\n", expConfig->videoSyncTip);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioBbLvlCtrl            = %d,\n", expConfig->audioBbLvlCtrl);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioFmAmpl               = %d,\n", expConfig->audioFmAmpl);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMainAmpThld          = %d,\n", expConfig->audioMainAmpThld);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioAmAmpThld            = %d,\n", expConfig->audioAmAmpThld);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMainPlusThld         = %d,\n", expConfig->audioMainPlusThld);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioMainPlusMaxAmp       = %d,\n", expConfig->audioMainPlusMaxAmp);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioAgcLimThld           = %d,\n", expConfig->audioAgcLimThld);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioAgcDecrIncr          = %d,\n", expConfig->audioAgcDecrIncr);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".audioAgcMinGain           = %d,\n", expConfig->audioAgcMinGain);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".jitterFilterOff           = %d,\n", expConfig->jitterFilterOff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".jitterFilterCoeff         = %d,\n", expConfig->jitterFilterCoeff);
    GT_1trace(VpsHalTrace, GT_DEBUG,
                ".jitterFilterEn            = %d,\n", expConfig->jitterFilterEn);

    GT_0trace(rfTrace, GT_LEAVE, "VpsHal_rfPrintExpertConfig<<<<");
}

