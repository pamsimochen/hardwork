/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPS_SII9135_PRIV_H_
#define _VPS_SII9135_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/sii9135/vpsdrv_sii9135.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

//#define VPS_SII9135_USE_REG_CACHE
//#define VPS_SII9135_USE_AVI_FRAME_INFO

/* Driver object state - NOT IN USE */
#define VPS_SII9135_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define VPS_SII9135_OBJ_STATE_IDLE     (1)

/* SII9135 I2C Port1 offset from Port0  */
#define VPS_SII9135_I2C_PORT1_OFFSET   (4)

/* SII9135 Default TMDS Core ID to use  */
#define VPS_SII9135_CORE_ID_DEFAULT    (0)

/* SII9135 Registers - I2C Port 0 */
#define VPS_SII9135_REG_VND_IDL         (0x00)
#define VPS_SII9135_REG_VND_IDH         (0x01)
#define VPS_SII9135_REG_DEV_IDL         (0x02)
#define VPS_SII9135_REG_DEV_IDH         (0x03)
#define VPS_SII9135_REG_DEV_REV         (0x04)
#define VPS_SII9135_REG_SW_RST_0        (0x05)
#define VPS_SII9135_REG_STATE           (0x06)
#define VPS_SII9135_REG_SW_RST_1        (0x07)
#define VPS_SII9135_REG_SYS_CTRL_1      (0x08)
#define VPS_SII9135_REG_SYS_SWTCHC      (0x09)
#define VPS_SII9135_REG_H_RESL          (0x3A)
#define VPS_SII9135_REG_H_RESH          (0x3B)
#define VPS_SII9135_REG_V_RESL          (0x3C)
#define VPS_SII9135_REG_V_RESH          (0x3D)
#define VPS_SII9135_REG_VID_CTRL        (0x48)
#define VPS_SII9135_REG_VID_MODE_2      (0x49)
#define VPS_SII9135_REG_VID_MODE_1      (0x4A)
#define VPS_SII9135_REG_VID_BLANK1      (0x4B)
#define VPS_SII9135_REG_VID_BLANK2      (0x4C)
#define VPS_SII9135_REG_VID_BLANK3      (0x4D)
#define VPS_SII9135_REG_DE_PIXL         (0x4E)
#define VPS_SII9135_REG_DE_PIXH         (0x4F)
#define VPS_SII9135_REG_DE_LINL         (0x50)
#define VPS_SII9135_REG_DE_LINH         (0x51)
#define VPS_SII9135_REG_VID_STAT        (0x55)
#define VPS_SII9135_REG_VID_CH_MAP      (0x56)
#define VPS_SII9135_REG_VID_XPCNTL      (0x6E)
#define VPS_SII9135_REG_VID_XPCNTH      (0x6F)

/* SII9135 Registers - I2C Port 1 */
#define VPS_SII9135_REG_SYS_PWR_DWN_2   (0x3E)
#define VPS_SII9135_REG_SYS_PWR_DWN     (0x3F)
#define VPS_SII9135_REG_AVI_TYPE        (0x40)
#define VPS_SII9135_REG_AVI_DBYTE15     (0x52)

/* SII9135 Registers values */

/* PCLK stable, Clock detect, Sync Detect */
#define VPS_SII9135_VID_DETECT          ((1<<0) | (1<<1) | (1<<4))

/* Video interlace status */
#define VPS_SII9135_VID_INTERLACE       (1<<2)

/* color format */
#define VPS_SII9135_COLOR_FORMAT_RGB    (0)
#define VPS_SII9135_COLOR_FORMAT_YUV    (1)

/* Select TMDS core */
#define VPS_SII9135_SEL_TMDS_CORE_0     ((1<<0)|(1<<4))
#define VPS_SII9135_SEL_TMDS_CORE_1     ((1<<1)|(1<<5))

/* F_xtal frequency in Khz  */
#define VPS_SII9135_FXTAL_KHZ           (27000)

/* HDMI AVI InfoFrame Packet info  */

/* color space  */
#define VPS_SII9135_AVI_INFO_COLOR_RGB444       (0)
#define VPS_SII9135_AVI_INFO_COLOR_YUV444       (1)
#define VPS_SII9135_AVI_INFO_COLOR_YUV422       (2)

/* color imetric  */
#define VPS_SII9135_AVI_INFO_CMETRIC_NO_DATA    (0)
#define VPS_SII9135_AVI_INFO_CMETRIC_ITU601     (1)
#define VPS_SII9135_AVI_INFO_CMETRIC_ITU709     (2)

/* pixel repition */
#define VPS_SII9135_AVI_INFO_PIXREP_NONE        (0)
#define VPS_SII9135_AVI_INFO_PIXREP_2X          (1)
#define VPS_SII9135_AVI_INFO_PIXREP_4X          (3)

/* AVI packet info values */
#define VPS_SII9135_AVI_INFO_PACKET_CODE        (0x82)
#define VPS_SII9135_AVI_INFO_VERSION_CODE       (0x02)
#define VPS_SII9135_AVI_INFO_PACKET_LENGTH      (0x0D)

/*
  Driver handle object
*/
typedef struct
{

    UInt32 state;               /* handle state */

    UInt32 handleId;            /* handle ID, 0..VPS_DEVICE_MAX_HANDLES-1 */

    Semaphore_Handle lock;      /* handle lock */

    Vps_VideoDecoderCreateParams createArgs;    /* create time arguments  */

    UInt8 regCache[2][256]; /* register read cache */

} Vps_Sii9135Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */

    Semaphore_Handle lock;      /* global driver lock */

    Vps_Sii9135Obj sii9135Obj[VPS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Vps_Sii9135CommonObj;

/*
  Information that is parsed from HDMI AVIInfoFrame packet
*/
typedef struct
{
    UInt32 colorSpace;          /* RGB444 ot YUV422 or YUV422 */
    UInt32 colorImetric;        /* BT709 or BT601 */
    UInt32 pixelRepeat;         /* 1x, 2x, 4x */

} Vps_Sii9135AviInfoFrame;

extern Vps_Sii9135CommonObj gVps_sii9135CommonObj;

Int32 Vps_sii9135GetChipId ( Vps_Sii9135Obj * pObj,
                             Vps_VideoDecoderChipIdParams * pPrm,
                             Vps_VideoDecoderChipIdStatus * pStatus );

Int32 Vps_sii9135GetVideoStatus ( Vps_Sii9135Obj * pObj,
                                  Vps_VideoDecoderVideoStatusParams * pPrm,
                                  Vps_VideoDecoderVideoStatus * pStatus );

Int32 Vps_sii9135Reset ( Vps_Sii9135Obj * pObj );

Int32 Vps_sii9135SetVideoMode ( Vps_Sii9135Obj * pObj,
                                Vps_VideoDecoderVideoModeParams * pPrm );

Int32 Vps_sii9135Start ( Vps_Sii9135Obj * pObj );

Int32 Vps_sii9135Stop ( Vps_Sii9135Obj * pObj );

Int32 Vps_sii9135RegWrite ( Vps_Sii9135Obj * pObj,
                            Vps_VideoDecoderRegRdWrParams * pPrm );

Int32 Vps_sii9135RegRead ( Vps_Sii9135Obj * pObj,
                           Vps_VideoDecoderRegRdWrParams * pPrm );

Int32 Vps_sii9135LockObj ( Vps_Sii9135Obj * pObj );
Int32 Vps_sii9135UnlockObj ( Vps_Sii9135Obj * pObj );
Int32 Vps_sii9135Lock (  );
Int32 Vps_sii9135Unlock (  );
Vps_Sii9135Obj *Vps_sii9135AllocObj (  );
Int32 Vps_sii9135FreeObj ( Vps_Sii9135Obj * pObj );

Int32 Vps_sii9135SetupVideo ( Vps_Sii9135Obj * pObj,
                              Vps_Sii9135AviInfoFrame * pAviInfo,
                              Vps_VideoDecoderVideoModeParams * pPrm );
Int32 Vps_sii9135ReadAviInfo ( Vps_Sii9135Obj * pObj,
                               Vps_Sii9135AviInfoFrame * pAviInfo );
Int32 Vps_sii9135DetectVideo ( Vps_Sii9135Obj * pObj,
                               Vps_Sii9135AviInfoFrame * pAviInfo,
                               UInt32 timeout );
Int32 Vps_sii9135SelectTmdsCore ( Vps_Sii9135Obj * pObj, UInt32 coreId );
Int32 Vps_sii9135PowerDown ( Vps_Sii9135Obj * pObj, UInt32 powerDown );
Int32 Vps_sii9135OutputEnable ( Vps_Sii9135Obj * pObj, UInt32 enable );

Int32 Vps_sii9135ResetRegCache(Vps_Sii9135Obj * pObj);

#endif /*  _VPS_SII9135_PRIV_H_  */
