/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_tvp7002Priv.h
 *
 *  \brief TVP7002 decoder internal header file.
 */

#ifndef _VPSDRV_TVP7002_PRIV_H_
#define _VPSDRV_TVP7002_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/devices/tvp7002/vpsdrv_tvp7002.h>
#include <ti/psp/platforms/vps_platform.h>

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Driver object state - NOT IN USE. */
#define VPS_TVP7002_OBJ_STATE_UNUSED    (0u)
/** \brief Driver object state - IN USE and IDLE. */
#define VPS_TVP7002_OBJ_STATE_IDLE      (1u)

/*
 * TVP7002 Register Offsets.
 */
#define VPS_TVP7002_REG_CHIP_REVISION               (0x00u)
#define VPS_TVP7002_REG_HPLL_DIVIDER_MSB            (0x01u)
#define VPS_TVP7002_REG_HPLL_DIVIDER_LSB            (0x02u)
#define VPS_TVP7002_REG_HPLL_CTRL                   (0x03u)
#define VPS_TVP7002_REG_HPLL_PHASE_SELECT           (0x04u)
#define VPS_TVP7002_REG_CLAMP_START                 (0x05u)
#define VPS_TVP7002_REG_CLAMP_WIDTH                 (0x06u)
#define VPS_TVP7002_REG_HSYNC_OUT_WIDTH             (0x07u)
#define VPS_TVP7002_REG_B_FINE_GAIN                 (0x08u)
#define VPS_TVP7002_REG_G_FINE_GAIN                 (0x09u)
#define VPS_TVP7002_REG_R_FINE_GAIN                 (0x0Au)
#define VPS_TVP7002_REG_B_FINE_OFFSET_MSB           (0x0Bu)
#define VPS_TVP7002_REG_G_FINE_OFFSET_MSB           (0x0Cu)
#define VPS_TVP7002_REG_R_FINE_OFFSET_MSB           (0x0Du)
#define VPS_TVP7002_REG_SYNC_CTRL1                  (0x0Eu)
#define VPS_TVP7002_REG_HPLL_CLAMP_CTRL             (0x0Fu)
#define VPS_TVP7002_REG_SOG_THRLD                   (0x10u)
#define VPS_TVP7002_REG_SYNC_SEPARATER_THRLD        (0x11u)
#define VPS_TVP7002_REG_HPLL_PRE_COAST              (0x12u)
#define VPS_TVP7002_REG_HPLL_POST_COAST             (0x13u)
#define VPS_TVP7002_REG_SYNC_DETECT_STATUS          (0x14u)
#define VPS_TVP7002_REG_OUT_FMT                     (0x15u)
#define VPS_TVP7002_REG_MISC_CTRL1                  (0x16u)
#define VPS_TVP7002_REG_MISC_CTRL2                  (0x17u)
#define VPS_TVP7002_REG_MISC_CTRL3                  (0x18u)
#define VPS_TVP7002_REG_IN_MUX_SELECT1              (0x19u)
#define VPS_TVP7002_REG_IN_MUX_SELECT2              (0x1Au)
#define VPS_TVP7002_REG_B_G_COARSE_GAIN             (0x1Bu)
#define VPS_TVP7002_REG_R_COARSE_GAIN               (0x1Cu)
#define VPS_TVP7002_REG_FINE_OFFSET_LSBS            (0x1Du)
#define VPS_TVP7002_REG_B_COARSE_OFFSET             (0x1Eu)
#define VPS_TVP7002_REG_G_COARSE_OFFSET             (0x1Fu)
#define VPS_TVP7002_REG_R_COARSE_OFFSET             (0x20u)
#define VPS_TVP7002_REG_HSOUT_OUT_START             (0x21u)
#define VPS_TVP7002_REG_MISC_CTRL4                  (0x22u)
#define VPS_TVP7002_REG_B_ALC_OUT_LSBS              (0x23u)
#define VPS_TVP7002_REG_G_ALC_OUT_LSBS              (0x24u)
#define VPS_TVP7002_REG_R_ALC_OUT_LSBS              (0x25u)
#define VPS_TVP7002_REG_ALC_ENABLE                  (0x26u)
#define VPS_TVP7002_REG_ALC_OUT_MSBS                (0x27u)
#define VPS_TVP7002_REG_ALC_FILTER                  (0x28u)
#define VPS_TVP7002_REG_RESERVED1                   (0x29u)
#define VPS_TVP7002_REG_FINE_CLAMP_CTRL             (0x2Au)
#define VPS_TVP7002_REG_POWER_CTRL                  (0x2Bu)
#define VPS_TVP7002_REG_ADC_SETUP                   (0x2Cu)
#define VPS_TVP7002_REG_COARSE_CLAMP_CTRL           (0x2Du)
#define VPS_TVP7002_REG_SOG_CLAMP                   (0x2Eu)
#define VPS_TVP7002_REG_RGB_COARSE_CLAMP_CTRL       (0x2Fu)
#define VPS_TVP7002_REG_SOG_COARSE_CLAMP_CTRL       (0x30u)
#define VPS_TVP7002_REG_ALC_PLACEMENT               (0x31u)
#define VPS_TVP7002_REG_RESERVED2                   (0x32u)
#define VPS_TVP7002_REG_RESERVED3                   (0x33u)
#define VPS_TVP7002_REG_MAC_STRIPPER_WIDTH          (0x34u)
#define VPS_TVP7002_REG_VSYNC_ALIGNMENT             (0x35u)
#define VPS_TVP7002_REG_SYNC_BYPASS                 (0x36u)
#define VPS_TVP7002_REG_LINES_PER_FRAME_STATUS_LSB  (0x37u)
#define VPS_TVP7002_REG_LINES_PER_FRAME_STATUS_MSB  (0x38u)
#define VPS_TVP7002_REG_CLOCK_PER_LINE_STATUS_LSB   (0x39u)
#define VPS_TVP7002_REG_CLOCK_PER_LINE_STATUS_MSB   (0x3Au)
#define VPS_TVP7002_REG_HSYNC_WIDTH                 (0x3Bu)
#define VPS_TVP7002_REG_VSYNC_WIDTH                 (0x3Cu)
#define VPS_TVP7002_REG_LINE_LENGTH_TOLERENCE       (0x3Du)
#define VPS_TVP7002_REG_RESERVED4                   (0x3Eu)
#define VPS_TVP7002_REG_VIDEO_BANDWIDTH_CTRL        (0x3Fu)
#define VPS_TVP7002_REG_AVID_START_PIXEL_LSB        (0x40u)
#define VPS_TVP7002_REG_AVID_START_PIXEL_MSB        (0x41u)
#define VPS_TVP7002_REG_AVID_STOP_PIXEL_LSB         (0x42u)
#define VPS_TVP7002_REG_AVID_STOP_PIXEL_MSB         (0x43u)
#define VPS_TVP7002_REG_VBLK_FLD0_START_OFFSET      (0x44u)
#define VPS_TVP7002_REG_VBLK_FLD1_START_OFFSET      (0x45u)
#define VPS_TVP7002_REG_VBLK_FLD0_DURATION          (0x46u)
#define VPS_TVP7002_REG_VBLK_FLD1_DURATION          (0x47u)
#define VPS_TVP7002_REG_FBIT_FLD0_START_OFFSET      (0x48u)
#define VPS_TVP7002_REG_FBIT_FLD1_START_OFFSET      (0x49u)
#define VPS_TVP7002_REG_CSC_COEFF0_LSB              (0x4Au)
/*
 * Shifts and masks for the fields.
 */
#define VPS_TVP7002_OUT_FMT_RANGE_SHIFT             (0x05u)
#define VPS_TVP7002_OUT_FMT_ORDER_SHIFT             (0x02u)
#define VPS_TVP7002_OUT_FMT_DECIMATION_SHIFT        (0x01u)
#define VPS_TVP7002_OUT_FMT_SYNC_ENABLE_SHIFT       (0x00u)

#define VPS_TVP7002_MISC_CTRL3_CSC_ENABLE_MASK      (0x10u)
#define VPS_TVP7002_MISC_CTRL3_CSC_ENABLE_SHIFT     (0x04u)

/** \brief RGB coding range (Y, Cb, and Cr range from 0 to 1023). */
#define VPS_TVP7002_RANGE_RGB               (0x00u)
/** \brief Extended coding range (Y, Cb, and Cr range from 4 to 1019). */
#define VPS_TVP7002_RANGE_EXTENDED          (0x01u)
/** \brief ITU-R BT.601 coding range (Y - 64 to 940, Cb/Cr - 64 to 960). */
#define VPS_TVP7002_RANGE_BT601             (0x02u)

/** \brief CbCr order. */
#define VPS_TVP7002_CBCR_ORDER              (0x00u)
/** \brief CrCb order. */
#define VPS_TVP7002_CRCB_ORDER              (0x01u)

/** \brief 30-bit 4:4:4 output format. */
#define VPS_TVP7002_OUT_FORMAT_30BIT        (0x00u)
/** \brief 30-bit 4:4:4 output format. */
#define VPS_TVP7002_OUT_FORMAT_20BIT        (0x01u)


/** \brief Number of CSC co-eff rgs in TVP7002 */
#define VPS_TVP7002_NUM_CSC_COEFF_REGS      (9*2)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief TVP7002 driver handle object.
 */
typedef struct
{
    UInt32                  state;
    /**< Handle state - used or free. */
    UInt32                  handleId;
    /**< Handle ID, 0..VPS_DEVICE_MAX_HANDLES-1. */
    Semaphore_Handle        lock;
    /**< Driver lock. */
    Vps_VideoDecoderCreateParams createArgs;
    /**< Create time arguments. */

} Vps_Tvp7002HandleObj;

/**
 *  \brief TVP7002 Global driver object.
 */
typedef struct
{
    Semaphore_Handle        lock;
    /* Global driver lock. */
    Vps_Tvp7002HandleObj    handlePool[VPS_DEVICE_MAX_HANDLES];
    /**< Handle objects. */
} Vps_Tvp7002Obj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 Vps_tvp7002ResetIoctl(Vps_Tvp7002HandleObj *pObj);
Int32 Vps_tvp7002GetChipIdIoctl(Vps_Tvp7002HandleObj *pObj,
                                Vps_VideoDecoderChipIdParams *pPrm,
                                Vps_VideoDecoderChipIdStatus *pStatus);
Int32 Vps_tvp7002SetVideoModeIoctl(Vps_Tvp7002HandleObj *pObj,
                                   Vps_VideoDecoderVideoModeParams *pPrm);
Int32 Vps_tvp7002GetVideoStatusIoctl(Vps_Tvp7002HandleObj *pObj,
                                     Vps_VideoDecoderVideoStatusParams *pPrm,
                                     Vps_VideoDecoderVideoStatus *pStatus);
Int32 Vps_tvp7002StartIoctl(Vps_Tvp7002HandleObj *pObj);
Int32 Vps_tvp7002StopIoctl(Vps_Tvp7002HandleObj *pObj);
Int32 Vps_tvp7002RegWriteIoctl(Vps_Tvp7002HandleObj *pObj,
                               Vps_VideoDecoderRegRdWrParams *pPrm);
Int32 Vps_tvp7002RegReadIoctl(Vps_Tvp7002HandleObj *pObj,
                              Vps_VideoDecoderRegRdWrParams *pPrm);

Int32 Vps_tvp7002I2cRead8(Vps_Tvp7002HandleObj *pObj,
                                 UInt32 i2cInstId,
                                 UInt32 i2cDevAddr,
                                 const UInt8 *regAddr,
                                 UInt8 *regVal,
                                 UInt32 numRegs);

Int32 Vps_tvp7002I2cWrite8(Vps_Tvp7002HandleObj *pObj,
                                  UInt32 i2cInstId,
                                  UInt32 i2cDevAddr,
                                  const UInt8 *regAddr,
                                  UInt8 *regVal,
                                  UInt32 numRegs);

Int32 Vps_tvp7002OutputEnable(Vps_Tvp7002HandleObj *pObj, UInt32 enable);

Int32 Vps_tvp7002PowerDown(Vps_Tvp7002HandleObj *pObj, UInt32 powerDown);

Int32 Vps_tvp7002SetupVideo(Vps_Tvp7002HandleObj *pObj,
                                   Vps_VideoDecoderVideoModeParams *pPrm);
Int32 Vps_tvp7002SetOutputFormat(Vps_Tvp7002HandleObj *pObj,
                                        Vps_VideoDecoderVideoModeParams *pPrm);

Int32 Vps_tvp7002ApplyCsc(Vps_Tvp7002HandleObj *pObj,
                                    UInt16 *cscCoeffs);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSDRV_TVP7002_PRIV_H_ */
