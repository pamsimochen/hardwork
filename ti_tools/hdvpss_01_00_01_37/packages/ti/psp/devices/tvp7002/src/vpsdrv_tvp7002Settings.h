/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_tvp7002Settings.h
 *
 *  \brief TVP7002 decoder internal header file for defining the register
 *  settings for various modes.
 */

#ifndef _VPSDRV_TVP7002_SETTINGS_H
#define _VPSDRV_TVP7002_SETTINGS_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief List of address offset used for common default settings.
 *  Caution: The address offset order should match with the value list order.
 */
static const UInt8 gTvp7002DefAddrList[] =
{
    VPS_TVP7002_REG_MISC_CTRL2
};

/**
 *  \brief List of values used for common default settings.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002DefValueList[] =
{
    0x13u               /* MISC_CTRL2               */
};

/**
 *  \brief List of address offset used for different standard settings.
 *  Caution: The address offset order should match with the value list order.
 */
static const UInt8 gTvp7002ModeAddrList[] =
{
    VPS_TVP7002_REG_HPLL_DIVIDER_MSB,
    VPS_TVP7002_REG_HPLL_DIVIDER_LSB,
    VPS_TVP7002_REG_HPLL_CTRL,
    VPS_TVP7002_REG_HPLL_PHASE_SELECT,
    VPS_TVP7002_REG_CLAMP_START,
    VPS_TVP7002_REG_CLAMP_WIDTH,
    VPS_TVP7002_REG_HSYNC_OUT_WIDTH,
    VPS_TVP7002_REG_SYNC_CTRL1,
    VPS_TVP7002_REG_HPLL_PRE_COAST,
    VPS_TVP7002_REG_HPLL_POST_COAST,
    VPS_TVP7002_REG_MISC_CTRL3,
    VPS_TVP7002_REG_IN_MUX_SELECT1,
    VPS_TVP7002_REG_IN_MUX_SELECT2,
    VPS_TVP7002_REG_HSOUT_OUT_START,
    VPS_TVP7002_REG_MISC_CTRL4,
    VPS_TVP7002_REG_ADC_SETUP,
    VPS_TVP7002_REG_ALC_PLACEMENT,
    VPS_TVP7002_REG_AVID_START_PIXEL_LSB,
    VPS_TVP7002_REG_AVID_START_PIXEL_MSB,
    VPS_TVP7002_REG_AVID_STOP_PIXEL_LSB,
    VPS_TVP7002_REG_AVID_STOP_PIXEL_MSB,
    VPS_TVP7002_REG_VBLK_FLD0_START_OFFSET,
    VPS_TVP7002_REG_VBLK_FLD1_START_OFFSET,
    VPS_TVP7002_REG_VBLK_FLD0_DURATION,
    VPS_TVP7002_REG_VBLK_FLD1_DURATION,
    VPS_TVP7002_REG_FBIT_FLD0_START_OFFSET,
    VPS_TVP7002_REG_FBIT_FLD1_START_OFFSET
};

/**
 *  \brief List of values used for 1080p60 settings in embedded sync mode.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002Value1080p60EmbSync[] =
{
    0x89u,              /* HPLL_DIVIDER_MSB         */
    0x80u,              /* HPLL_DIVIDER_LSB         */
    0xE0u,              /* HPLL_CTRL                */
    0xA0u,              /* HPLL_PHASE_SELECT        */
    0x32u,              /* CLAMP_START              */
    0x20u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x5Bu,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0x00u,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x08u,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x80u,              /* ADC_SETUP                */
    0x5Au,              /* ALC_PLACEMENT            */
    0x00u,              /* AVID_START_PIXEL_LSB     */
    0x01u,              /* AVID_START_PIXEL_MSB     */
    0x84u,              /* AVID_STOP_PIXEL_LSB      */
    0x08u,              /* AVID_STOP_PIXEL_MSB      */
    0x05u,              /* VBLK_FLD0_START_OFFSET   */
    0x00u,              /* VBLK_FLD1_START_OFFSET   */
    0x2Du,              /* VBLK_FLD0_DURATION       */
    0x00u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};

/**
 *  \brief List of values used for 1080p60 settings in discrete sync mode.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002Value1080p60DiscSync[] =
{
    0x89u,              /* HPLL_DIVIDER_MSB         */
    0x80u,              /* HPLL_DIVIDER_LSB         */
    0xE0u,              /* HPLL_CTRL                */
    0xA0u,              /* HPLL_PHASE_SELECT        */
    0x32u,              /* CLAMP_START              */
    0x20u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x5Bu,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0x00u,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x08u,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x80u,              /* ADC_SETUP                */
    0x5Au,              /* ALC_PLACEMENT            */
    0xF4u,              /* AVID_START_PIXEL_LSB     */
    0x00u,              /* AVID_START_PIXEL_MSB     */
    0x74u,              /* AVID_STOP_PIXEL_LSB      */
    0x08u,              /* AVID_STOP_PIXEL_MSB      */
    0x05u,              /* VBLK_FLD0_START_OFFSET   */
    0x00u,              /* VBLK_FLD1_START_OFFSET   */
    0x2Du,              /* VBLK_FLD0_DURATION       */
    0x00u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};

/**
 *  \brief List of values used for 1080i60 settings in embedded sync mode.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002Value1080i60EmbSync[] =
{
    0x89u,              /* HPLL_DIVIDER_MSB         */
    0x80u,              /* HPLL_DIVIDER_LSB         */
    0x98u,              /* HPLL_CTRL                */
    0xA0u,              /* HPLL_PHASE_SELECT        */
    0x32u,              /* CLAMP_START              */
    0x20u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x5Bu,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0x00u,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x0Du,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x50u,              /* ADC_SETUP                */
    0x5Au,              /* ALC_PLACEMENT            */
    0xF1u,              /* AVID_START_PIXEL_LSB     */
    0x00u,              /* AVID_START_PIXEL_MSB     */
    0x75u,              /* AVID_STOP_PIXEL_LSB      */
    0x08u,              /* AVID_STOP_PIXEL_MSB      */
    0x02u,              /* VBLK_FLD0_START_OFFSET   */
    0x02u,              /* VBLK_FLD1_START_OFFSET   */
    0x16u,              /* VBLK_FLD0_DURATION       */
    0x17u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};

/**
 *  \brief List of values used for 1080i60 settings in discrete sync mode.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002Value1080i60DiscSync[] =
{
    0x89u,              /* HPLL_DIVIDER_MSB         */
    0x80u,              /* HPLL_DIVIDER_LSB         */
    0x98u,              /* HPLL_CTRL                */
    0xA0u,              /* HPLL_PHASE_SELECT        */
    0x32u,              /* CLAMP_START              */
    0x20u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x5Bu,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0x00u,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x0Du,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x50u,              /* ADC_SETUP                */
    0x5Au,              /* ALC_PLACEMENT            */
    0xF4u,              /* AVID_START_PIXEL_LSB     */
    0x00u,              /* AVID_START_PIXEL_MSB     */
    0x74u,              /* AVID_STOP_PIXEL_LSB      */
    0x08u,              /* AVID_STOP_PIXEL_MSB      */
    0x02u,              /* VBLK_FLD0_START_OFFSET   */
    0x02u,              /* VBLK_FLD1_START_OFFSET   */
    0x16u,              /* VBLK_FLD0_DURATION       */
    0x17u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};

/**
 *  \brief List of values used for 720p60 settings in embedded sync mode.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002Value720p60EmbSync[] =
{
    0x67u,              /* HPLL_DIVIDER_MSB         */
    0x20u,              /* HPLL_DIVIDER_LSB         */
    0xA0u,              /* HPLL_CTRL                */
    0xB0u,              /* HPLL_PHASE_SELECT        */
    0x32u,              /* CLAMP_START              */
    0x20u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x5Bu,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0x00u,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x08u,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x50u,              /* ADC_SETUP                */
    0x5Au,              /* ALC_PLACEMENT            */
    0x47u,              /* AVID_START_PIXEL_LSB     */
    0x01u,              /* AVID_START_PIXEL_MSB     */
    0x4Bu,              /* AVID_STOP_PIXEL_LSB      */
    0x06u,              /* AVID_STOP_PIXEL_MSB      */
    0x05u,              /* VBLK_FLD0_START_OFFSET   */
    0x00u,              /* VBLK_FLD1_START_OFFSET   */
    0x1Eu,              /* VBLK_FLD0_DURATION       */
    0x00u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};

/**
 *  \brief List of values used for 720p60 settings in discrete sync mode.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002Value720p60DiscSync[] =
{
    0x67u,              /* HPLL_DIVIDER_MSB         */
    0x20u,              /* HPLL_DIVIDER_LSB         */
    0xA0u,              /* HPLL_CTRL                */
    0xB0u,              /* HPLL_PHASE_SELECT        */
    0x32u,              /* CLAMP_START              */
    0x20u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x5Bu,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0x00u,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x08u,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x50u,              /* ADC_SETUP                */
    0x5Au,              /* ALC_PLACEMENT            */
    0x35u,              /* AVID_START_PIXEL_LSB     */
    0x01u,              /* AVID_START_PIXEL_MSB     */
    0x35u,              /* AVID_STOP_PIXEL_LSB      */
    0x06u,              /* AVID_STOP_PIXEL_MSB      */
    0x05u,              /* VBLK_FLD0_START_OFFSET   */
    0x00u,              /* VBLK_FLD1_START_OFFSET   */
    0x1Eu,              /* VBLK_FLD0_DURATION       */
    0x00u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};

/**
 *  \brief List of values used for SXGA@60 settings.
 *  Caution: The value list order should match with the address list order.
 */
static UInt8 gTvp7002ValueSxga60[] =
{
    0x69u,              /* HPLL_DIVIDER_MSB         */
    0x80u,              /* HPLL_DIVIDER_LSB         */
    0xA0u,              /* HPLL_CTRL                */
    0x80u,              /* HPLL_PHASE_SELECT        */
    0x06u,              /* CLAMP_START              */
    0x10u,              /* CLAMP_WIDTH              */
    0x20u,              /* HSYNC_OUT_WIDTH          */
    0x52u,              /* SYNC_CTRL1               */
    0x01u,              /* HPLL_PRE_COAST           */
    0x00u,              /* HPLL_POST_COAST          */
    0x00u,              /* MISC_CTRL3               */
    0xAAu,              /* IN_MUX_SELECT1           */
    0x6Au,              /* IN_MUX_SELECT2           */
    0x08u,              /* HSOUT_OUT_START          */
    0x00u,              /* MISC_CTRL4               */
    0x50u,              /* ADC_SETUP                */
    0x18u,              /* ALC_PLACEMENT            */
    0x80u,              /* AVID_START_PIXEL_LSB     */
    0x01u,              /* AVID_START_PIXEL_MSB     */
    0x86u,              /* AVID_STOP_PIXEL_LSB      */
    0x06u,              /* AVID_STOP_PIXEL_MSB      */
    0x04u,              /* VBLK_FLD0_START_OFFSET   */
    0x00u,              /* VBLK_FLD1_START_OFFSET   */
    0x2Au,              /* VBLK_FLD0_DURATION       */
    0x00u,              /* VBLK_FLD1_DURATION       */
    0x00u,              /* FBIT_FLD0_START_OFFSET   */
    0x00u               /* FBIT_FLD1_START_OFFSET   */
};


UInt16 gTvp7002CscCoeffRgb2Yuv[] =
{
    0x16E3, 0x024F, 0x06CE,
    0xF3AB, 0x1000, 0xFC55,
    0xF178, 0xFE88, 0x1000,
};

UInt16 gTvp7002CscCoeffYuv2Rgb[] =
{
    0x2000, 0xF538, 0xE9A8,
    0x2000, 0x0000, 0x2BE8,
    0x2000, 0x3778, 0x0000,

};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSDRV_TVP7002_SETTINGS_H */
