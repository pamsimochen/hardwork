/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPS_TVP5158_PRIV_H_
#define _VPS_TVP5158_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/tvp5158/vpsdrv_tvp5158.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#define VPS_TVP5158_DEBUG
//#define VPS_TVP5158_VERBOSE_DEBUG

//#define VPS_TVP5158_ENABLE_NF
#define VPS_TVP5158_ENABLE_FIRMWARE_PATCHES

#define VPS_TVP5158_ENABLE_COMB_FILTER_SETUP

/* Uncomment this to NOT download TVP5158 patch */
/*#define VPS_TVP5158_NO_PATCH_DOWNLOAD*/

/* Uncomment this to FORCE TVP5158 patch download each time
  IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE is called
*/
/*#define VPS_TVP5158_FORCE_PATCH_DOWNLOAD*/

/* Max possible channel per TVP5158 device  */
#define VPS_TVP5158_CH_PER_DEVICE_MAX    (4)

/* Max possible devices cascade together to feed video data to one video port */
#define VPS_TVP5158_DEV_PER_PORT_MAX   (2)

/* TVP5158 handle state - not in use  */
#define VPS_TVP5158_OBJ_STATE_UNUSED   (0)

/* TVP5158 handle state - in use and in idle state  */
#define VPS_TVP5158_OBJ_STATE_IDLE     (1)

/* Apply settings to all core's */
#define VPS_TVP5158_CORE_ALL    (0xF)

/* TVP5158 registers  */
#define VPS_TVP5158_REG_STATUS_1        (0x00)
#define VPS_TVP5158_REG_STATUS_2        (0x01)
#define VPS_TVP5158_REG_ROM_VERSION     (0x04)
#define VPS_TVP5158_REG_RAM_VERSION_0   (0x05)
#define VPS_TVP5158_REG_RAM_VERSION_1   (0x06)
#define VPS_TVP5158_REG_CHIP_ID_MSB     (0x08)
#define VPS_TVP5158_REG_CHIP_ID_LSB     (0x09)
#define VPS_TVP5158_REG_VID_STD_STATUS  (0x0C)
#define VPS_TVP5158_REG_VID_STD_SELECT  (0x0D)
#define VPS_TVP5158_REG_AUTO_SW_MASK    (0x0E)
#define VPS_TVP5158_REG_Y_BRIGHTNESS    (0x10)
#define VPS_TVP5158_REG_Y_CONTRAST      (0x11)
#define VPS_TVP5158_REG_C_SATURATION    (0x13)
#define VPS_TVP5158_REG_C_HUE           (0x14)
#define VPS_TVP5158_REG_Y_CTRL_1        (0x18)
#define VPS_TVP5158_REG_Y_CTRL_2        (0x19)
#define VPS_TVP5158_REG_C_CTRL_1        (0x1B)
#define VPS_TVP5158_REG_C_CTRL_2        (0x1C)
#define VPS_TVP5158_REG_NR_MAX_NOISE    (0x5C)
#define VPS_TVP5158_REG_NR_CTRL         (0x5D)
#define VPS_TVP5158_REG_OP_MODE_CTRL    (0x60)
#define VPS_TVP5158_REG_FV_DEC_CTRL     (0x88)
#define VPS_TVP5158_REG_FV_CTRL         (0x89)
#define VPS_TVP5158_REG_FBIT_DURATION   (0x8B)
#define VPS_TVP5158_REG_ESYNC_OFFSET_1  (0xAE)
#define VPS_TVP5158_REG_ESYNC_OFFSET_2  (0xAF)
#define VPS_TVP5158_REG_AVD_OUT_CTRL_1  (0xB0)
#define VPS_TVP5158_REG_AVD_OUT_CTRL_2  (0xB1)
#define VPS_TVP5158_REG_OFM_MODE_CTRL   (0xB2)
#define VPS_TVP5158_REG_OFM_CH_SEL_1    (0xB3)
#define VPS_TVP5158_REG_OFM_CH_SEL_2    (0xB4)
#define VPS_TVP5158_REG_OFM_CH_SEL_3    (0xB5)
#define VPS_TVP5158_REG_OFM_SFRAME_LSB  (0xB6)
#define VPS_TVP5158_REG_OFM_SFRAME_MSB  (0xB7)
#define VPS_TVP5158_REG_OFM_HBLANK_LSB  (0xB8)
#define VPS_TVP5158_REG_OFM_HBLANK_MSB  (0xB9)
#define VPS_TVP5158_REG_MISC_OFM_CTRL   (0xBA)
#define VPS_TVP5158_REG_AUDIO_SAMPLE_HZ (0xC0)
#define VPS_TVP5158_REG_AUDIO_GAIN_1    (0xC1)
#define VPS_TVP5158_REG_AUDIO_GAIN_2    (0xC2)
#define VPS_TVP5158_REG_AUDIO_CTRL      (0xC3)
#define VPS_TVP5158_REG_AUDIO_MIXER     (0xC4)
#define VPS_TVP5158_REG_AUDIO_CASCADE   (0xC8)
#define VPS_TVP5158_REG_MIN_F1_ACT      (0xD5)
#define VPS_TVP5158_REG_DEC_WR_EN       (0xFE)
#define VPS_TVP5158_REG_DEC_RD_EN       (0xFF)

/* TVP5158 register field values  */

/* Mode of operation  */
#define VPS_TVP5158_NON_INTERLEAVED_MODE          (0)
#define VPS_TVP5158_PIXEL_INTERLEAVED_MODE        (1)
#define VPS_TVP5158_LINE_INTERLEAVED_MODE         (2)
#define VPS_TVP5158_LINE_INTERLEAVED_HYBRID_MODE  (3)

/* Number of channels */
#define VPS_TVP5158_1CH_MUX                       (0)
#define VPS_TVP5158_2CH_MUX                       (1)
#define VPS_TVP5158_4CH_MUX                       (2)
#define VPS_TVP5158_8CH_MUX                       (3)

/* Resolution */
#define VPS_TVP5158_RES_D1                        (0)
#define VPS_TVP5158_RES_HALF_D1                   (2)
#define VPS_TVP5158_RES_CIF                       (3)

/* 8-bit or 16-bit data width */
#define VPS_TVP5158_OUT_TYPE_8BIT                 (0)
#define VPS_TVP5158_OUT_TYPE_16BIT                (1)

/* Super-frame size type  */
#define VPS_TVP5158_SUPER_FRAME_SIZE_525_STD      (0)
#define VPS_TVP5158_SUPER_FRAME_SIZE_625_STD      (1)
#define VPS_TVP5158_SUPER_FRAME_SIZE_MANUAL       (3)

/* Bit pattern to enable TVP5158 output */
#define VPS_TVP5158_OUT_ENABLE      (0x05)

/* TVP5158 status */
#define VPS_TVP5158_HSYNC_LOCKED    (0x02)
#define VPS_TVP5158_VSYNC_LOCKED    (0x04)
#define VPS_TVP5158_SIGNAL_DETECT   (0x80)

#define VPS_TVP5158_SIGNAL_60HZ     (0x20)

#define VPS_TVP5158_VID_STD_MASK    (0x07)

/* TVP5158 video standard's */
#define VPS_TVP5158_VID_STD_PAL_BDGHIN  (0x02)
#define VPS_TVP5158_VID_STD_PAL_M       (0x03)
#define VPS_TVP5158_VID_STD_PAL_COMB_N  (0x04)
#define VPS_TVP5158_VID_STD_PAL_60      (0x07)
#define VPS_TVP5158_VID_STD_NTSC_MJ     (0x01)
#define VPS_TVP5158_VID_STD_NTSC_4_43   (0x05)

#define VPS_TVP5158_NTSC_PAL_WIDTH      (720)
#define VPS_TVP5158_NTSC_HEIGHT         (240)
#define VPS_TVP5158_PAL_HEIGHT          (288)

/* Audio sampling rate  */
#define VPS_TVP5158_AUDIO_16KHZ                   (0)
#define VPS_TVP5158_AUDIO_8KHZ                    (2)

/* Audio TDM mode */
#define VPS_TVP5158_AUDIO_TDM_2CH       (0x0)
#define VPS_TVP5158_AUDIO_TDM_4CH       (0x1)
#define VPS_TVP5158_AUDIO_TDM_8CH       (0x2)
#define VPS_TVP5158_AUDIO_TDM_12CH      (0x3)
#define VPS_TVP5158_AUDIO_TDM_16CH      (0x4)

/*
  TVP5158 Object
*/
typedef struct
{

    UInt32 state;               /* object state VPS_TVP5158_OBJ_STATE_xxx */

    UInt32 handleId;            /* handle ID, 0..VPS_DEVICE_MAX_HANDLES-1 */

    Semaphore_Handle lock;      /* handle lock */

    Vps_VideoDecoderVideoModeParams videoModeParams; /* video mode params */

    Vps_VideoDecoderCreateParams createArgs;    /* create time arguments */

} Vps_Tvp5158Obj;

/*
  Overall TVP5158 driver object
*/
typedef struct
{

    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */

    Semaphore_Handle lock;      /* global level lock */

    Vps_Tvp5158Obj tvp5158Obj[VPS_DEVICE_MAX_HANDLES];
    /*
     * TVP5158 handle objects
     */

} Vps_Tvp5158CommonObj;

extern Vps_Tvp5158CommonObj gVps_tvp5158CommonObj;

Int32 Vps_tvp5158GetChipId ( Vps_Tvp5158Obj * pObj,
                             Vps_VideoDecoderChipIdParams * pPrm,
                             Vps_VideoDecoderChipIdStatus * pStatus );

Int32 Vps_tvp5158GetVideoStatus ( Vps_Tvp5158Obj * pObj,
                                  Vps_VideoDecoderVideoStatusParams * pPrm,
                                  Vps_VideoDecoderVideoStatus * pStatus );

Int32 Vps_tvp5158Reset ( Vps_Tvp5158Obj * pObj );

Int32 Vps_tvp5158SetVideoMode ( Vps_Tvp5158Obj * pObj,
                                Vps_VideoDecoderVideoModeParams * pPrm );

Int32 Vps_tvp5158Start ( Vps_Tvp5158Obj * pObj );

Int32 Vps_tvp5158Stop ( Vps_Tvp5158Obj * pObj );

Int32 Vps_tvp5158SetVideoColor ( Vps_Tvp5158Obj * pObj,
                                 Vps_VideoDecoderColorParams * pPrm );

Int32 Vps_tvp5158SetVideoNf ( Vps_Tvp5158Obj * pObj,
                              Vps_Tvp5158VideoNfParams * pPrm );

Int32 Vps_tvp5158RegWrite ( Vps_Tvp5158Obj * pObj,
                            Vps_VideoDecoderRegRdWrParams * pPrm );

Int32 Vps_tvp5158RegRead ( Vps_Tvp5158Obj * pObj,
                           Vps_VideoDecoderRegRdWrParams * pPrm );

Int32 Vps_tvp5158LockObj ( Vps_Tvp5158Obj * pObj );
Int32 Vps_tvp5158UnlockObj ( Vps_Tvp5158Obj * pObj );
Int32 Vps_tvp5158Lock (  );
Int32 Vps_tvp5158Unlock (  );
Vps_Tvp5158Obj *Vps_tvp5158AllocObj (  );
Int32 Vps_tvp5158FreeObj ( Vps_Tvp5158Obj * pObj );

Int32 Vps_tvp5158OfmReset ( Vps_Tvp5158Obj * pObj);

Int32 Vps_tvp5158SelectWrite ( Vps_Tvp5158Obj * pObj,
                               UInt32 devId, UInt32 value );

Int32 Vps_tvp5158VbusWrite ( Vps_Tvp5158Obj * pObj,
                             UInt32 devId, UInt32 vbusAddr, UInt8 val,
                             UInt32 len );

Int32 Vps_tvp5158VbusRead ( Vps_Tvp5158Obj * pObj,
                            UInt32 devId, UInt32 vbusAddr, UInt8 * val );

Int32 Vps_tvp5158PatchDownload ( Vps_Tvp5158Obj * pObj, UInt32 devId );

Int32 Vps_tvp5158OutputEnable ( Vps_Tvp5158Obj * pObj, UInt32 enable );

Int32 Vps_tvp5158CheckVideoSettings ( UInt32 interleaveMode,
                                      UInt32 chMuxNum, UInt32 vidResSel,
                                      UInt32 outputType );

Int32 Vps_tvp5158SetAudioMode ( Vps_Tvp5158Obj * pObj,
                                Vps_Tvp5158AudioModeParams * pPrm );

Int32 Vps_tvp5158SetAudioVolume ( Vps_Tvp5158Obj * pObj,
                                  UInt32 channelNum, UInt32 audioVolume );

Int32 Vps_tvp5158NfEnableAll(Vps_Tvp5158Obj * pObj, Bool enable);

Int32 Vps_tvp5158PrintChipId ( Vps_Tvp5158Obj * pObj);

Int32 Vps_tvp5158SetExtendedSettings( Vps_Tvp5158Obj * pObj);

Int32 Vps_tvp5158SetIndirectRegisters(Vps_Tvp5158Obj * pObj);

#endif /*  _VPS_TVP5158_PRIV_H_  */
