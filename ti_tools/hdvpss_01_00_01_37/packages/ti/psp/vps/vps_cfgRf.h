/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_VPS_COMMON
 * \addtogroup VPS_DRV_FVID2_VPS_COMMON_RF HD-VPSS - RF Config API
 *
 * @{
 */

/**
 *  \file vps_cfgRf.h
 *
 *  \brief HD-VPSS - RF Config API
 */

#ifndef _VPS_CFG_RF_H
#define _VPS_CFG_RF_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Configuration parameters for RF Modulator.
 */
typedef struct
{
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

    UInt32              audioInVol;
    /**< Audio input volume control. 0x0000 = min, 0xFFFF = max. */

    UInt32              audioMute;
    /**< Mute audio. 0 = Inactive (non-mute), 1 = Active (mute). */

    UInt32              audioIsMono;
    /**< Mono / Stereo audio. 0 = Stereo, 1 = Mono. */
} Vps_RfConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#endif

/* @} */

