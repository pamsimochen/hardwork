/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshalRfDefaults.h
 *
 * \brief VPS RF HAL default configuration file
 * This file contains default configuration i.e. expert values for
 * RF Modulator.
 *
 */

#ifndef _VPSHALRFDEFAULTS_H
#define _VPSHALRFDEFAULTS_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/hal/vpshal_rf.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Num of coefficients in each set */
#define VPSHAL_RF_NUM_COEFF             (25u)

/**
 *  \brief Expert value for the RF Modulator
 */
#define VPSHAL_RF_DEFAULT_EXPERT_VAL                                           \
{                                                                              \
    /* video low pass filter coefficients */                                   \
    0xFAD3u, 0x01B3u, 0xFC75u, 0x015Au, 0xFBFAu,                               \
    0x0363u, 0xFBC3u, 0x035Eu, 0xFF2Cu, 0xFFC4u,                               \
    0x0083u, 0xFF80u, 0x0083u, 0x033Du, 0xFF4Eu,                               \
    0xFF44u, 0x0379u, 0xFC6Eu, 0x0376u, 0xFEDBu,                               \
    0xFFA6u, 0x04A0u, 0xFD31u, 0x04C9u, 0x00F6u,                               \
    /* video group delay pre-correction coefficients */                        \
    0xF961u, 0x02F5u, 0xF9C1u, 0x0304u, 0xFAF6u,                               \
    0x0327u, 0xFA4Eu, 0x0315u, 0xF939u, 0x02E5u,                               \
    0x02F5u, 0xF961u, 0x0400u, 0x0304u, 0xF9C1u,                               \
    0x0400u, 0x0327u, 0xFAF6u, 0x0400u, 0x0315u,                               \
    0xFA4Eu, 0x0400u, 0x02E5u, 0xF939u, 0x0400u,                               \
    0x03FB,         /* Wideband & Spectral Normalization Reg0 */               \
    0x0C47,         /* Wideband & Spectral Normalization Reg1 */               \
    0xB4CD,         /* Audio Main Max Amplitude Reg0 */                        \
    0xB4CD,         /* Audio Stereo Max Amplitude Reg1 */                      \
    0x795E,         /* Audio Main  FM Gain Reg0 */                             \
    0x795E,         /* Audio Stereo FM Gain Reg1 */                            \
    0x0002,         /* Audio Main Variable Delay Reg0 */                       \
    0x0000,         /* Audio Stereo Variable Delay Reg1 */                     \
    0x0200,         /* Audio Stereo Over modulation Limit Reg */               \
    0x0038,         /* Audio Volume and Mute Control Reg - volume */           \
    TRUE,           /* Audio Volume and Mute Control Reg - mute */             \
    FALSE,          /* Audio Volume and Mute Control Reg - mono */             \
    FALSE,          /* Control and Channel 3&4 select - ch3_4 */               \
    FALSE,          /* Control and Channel 3&4 select - dac_rst_pol */         \
    FALSE,          /* Control and Channel 3&4 select - dac_on_off */          \
    FALSE,          /* Control and Channel 3&4 select - dac_data_zero */       \
    FALSE,          /* Control and Channel 3&4 select - dac_data_invert */     \
    0x01B0,         /* Video RF Amplitude Reg */                               \
    0x039F,         /* Video RF Gain Reg */                                    \
    0x0000,         /* Video sync tip LSBs */                                  \
    0x0100,         /* Audio Baseband Level Control Reg */                     \
    0x0036,         /* Audio FM Amplitude Reg */                               \
    0x7FFF,         /* Audio main amplitude threshold Reg */                   \
    0x7FFF,         /* Stereo AM amplitude threshold Reg */                    \
    0x0F2C,         /* Audio main plus stereo amplitude threshold */           \
    0x156E,         /* Audio main plus stereo max amplitude */                 \
    0x2666,         /* Audio agc limit threshold */                            \
    0x6401,         /* Audio agc gain decrement and increment */               \
    0x019A,         /* Audio agc minimum gain limit */                         \
    0x007C,         /* Jitter pre-correction filter offset */                  \
    0x0020,         /* Jitter pre-correction filter coefficient */             \
    0x0000,         /* Jitter pre-correction filter enable */                  \
}

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief Set the Coefficients for RF Modulator.
 */
typedef struct
{
    UInt32              videoLpfCoeff[VPSHAL_RF_NUM_COEFF];
    /**< Programmable video low pass filter coefficients. */

    UInt32              videoGdpCoeff[VPSHAL_RF_NUM_COEFF];
    /**< Programmable video group delay pre-correction coefficients. */
} VpsHal_RfCoeff;

/**
 * \brief Set the expert configuration for RF Modulator.
 */
typedef struct
{
    VpsHal_RfCoeff      rfCoeff;
    /**< Programmable video coefficients. */

    UInt32              wbSpecNormal0;
    /**< Wideband & Spectral Normalization Reg0 */

    UInt32              wbSpecNormal1;
    /**< Wideband & Spectral Normalization Reg1 */

    UInt32              audioMainMaxAmp;
    /**< Audio Main Max Amplitude Reg0 */

    UInt32              audioStereoMaxAmp;
    /**< Audio Stereo Max Amplitude Reg1 */

    UInt32              audioMainFmGain;
    /**< Audio Main FM Gain Reg0 */

    UInt32              audioStereoFmGain;
    /**< Audio Stereo FM Gain Reg1 */

    UInt32              audioMainVarDelay;
    /**< Audio Main Variable Delay Reg0 */

    UInt32              audioStereoVarDelay;
    /**< Audio Stereo Variable Delay Reg1 */

    UInt32              audioStereoOvermodLimit;
    /**< Audio Stereo Overmodulation Limit Reg */

    UInt32              audioInVol;
    /**< Audio input volume control. 0x0000 = min, 0xFFFF = max. */

    UInt32              audioMute;
    /**< Mute audio. 0 = Inactive (non-mute), 1 = Active (mute). */

    UInt32              audioIsMono;
    /**< Mono / Stereo audio. 0 = Stereo, 1 = Mono. */

    UInt32              channel34Select;
    /**< Used to select the channel. 0 = Ch3, 1 = Ch4 */

    UInt32              dacResetPolarity;
    /**< Polarity of reset signal to DAC.
         0 = Active low, 1= Active high */

    UInt32              dacOnOff;
    /**< DAC on/off. 0 = Off, 1 = On. */

    UInt32              dacDataZero;
    /**< Zero DAC data input. 0 = Inactive, 1 = Active. */

    UInt32              dacDataInvert;
    /**< Invert DAC input data path. 0 = Inactive, 1 = Active. */

    UInt32              videoRfAmp;
    /**< Video RF Amplitude Reg */

    UInt32              videoRfGain;
    /**< Video RF Gain Reg */

    UInt32              videoSyncTip;
    /**< Video sync tip LSBs */

    UInt32              audioBbLvlCtrl;
    /**< Audio Baseband Level Control Reg */

    UInt32              audioFmAmpl;
    /**<  Audio FM Amplitude Reg */

    UInt32              audioMainAmpThld;
    /**< Audio main amplitude threshold Reg */

    UInt32              audioAmAmpThld;
    /**< Audio Stereo AM amplitude threshold Reg */

    UInt32              audioMainPlusThld;
    /**< Audio main plus stereo amplitude threshold */

    UInt32              audioMainPlusMaxAmp;
    /**< Audio main plus stereo max amplitude */

    UInt32              audioAgcLimThld;
    /**< Audio AGC limit threshold */

    UInt32              audioAgcDecrIncr;
    /**< Audio AGC decrement and increment */

    UInt32              audioAgcMinGain;
    /**< Audio AGC minimum gain limit */

    UInt32              jitterFilterOff;
    /**< Jitter pre-correction filter offset */

    UInt32              jitterFilterCoeff;
    /**< Jitter pre-correction filter coefficient */

    UInt32              jitterFilterEn;
    /**< Jitter pre-correction filter enable */
} VpsHal_RfExpertConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHALRFDEFAULTS_H */
