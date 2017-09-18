/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISS_AR0330_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_AR0330_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/ar0330/issdrv_ar0330.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

//#define ISS_AR0330_USE_REG_CACHE
//#define ISS_AR0330_USE_AVI_FRAME_INFO

/* Driver object state - NOT IN USE */
#define ISS_AR0330_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_AR0330_OBJ_STATE_IDLE     (1)

/* AR0330 I2C Port1 offset from Port0  */
#define ISS_AR0330_I2C_PORT1_OFFSET   (4)

/* AR0330 Default TMDS Core ID to use  */
#define ISS_AR0330_CORE_ID_DEFAULT    (0)

/* AR0330 Registers - I2C Port 0 */
#define ISS_AR0330_REG_VND_IDL         (0x00)
#define ISS_AR0330_REG_VND_IDH         (0x01)
#define ISS_AR0330_REG_DEV_IDL         (0x02)
#define ISS_AR0330_REG_DEV_IDH         (0x03)
#define ISS_AR0330_REG_DEV_REV         (0x04)
#define ISS_AR0330_REG_SW_RST_0        (0x05)
#define ISS_AR0330_REG_STATE           (0x06)
#define ISS_AR0330_REG_SW_RST_1        (0x07)
#define ISS_AR0330_REG_SYS_CTRL_1      (0x08)
#define ISS_AR0330_REG_SYS_SWTCHC      (0x09)
#define ISS_AR0330_REG_H_RESL          (0x3A)
#define ISS_AR0330_REG_H_RESH          (0x3B)
#define ISS_AR0330_REG_V_RESL          (0x3C)
#define ISS_AR0330_REG_V_RESH          (0x3D)
#define ISS_AR0330_REG_VID_CTRL        (0x48)
#define ISS_AR0330_REG_VID_MODE_2      (0x49)
#define ISS_AR0330_REG_VID_MODE_1      (0x4A)
#define ISS_AR0330_REG_VID_BLANK1      (0x4B)
#define ISS_AR0330_REG_VID_BLANK2      (0x4C)
#define ISS_AR0330_REG_VID_BLANK3      (0x4D)
#define ISS_AR0330_REG_DE_PIXL         (0x4E)
#define ISS_AR0330_REG_DE_PIXH         (0x4F)
#define ISS_AR0330_REG_DE_LINL         (0x50)
#define ISS_AR0330_REG_DE_LINH         (0x51)
#define ISS_AR0330_REG_VID_STAT        (0x55)
#define ISS_AR0330_REG_VID_CH_MAP      (0x56)
#define ISS_AR0330_REG_VID_XPCNTL      (0x6E)
#define ISS_AR0330_REG_VID_XPCNTH      (0x6F)

/* AR0330 Registers - I2C Port 1 */
#define ISS_AR0330_REG_SYS_PWR_DWN_2   (0x3E)
#define ISS_AR0330_REG_SYS_PWR_DWN     (0x3F)
#define ISS_AR0330_REG_AVI_TYPE        (0x40)
#define ISS_AR0330_REG_AVI_DBYTE15     (0x52)

#define     AR_0330_I2C_DEV_ADDR				(AR_0330_ADDR)
#define     AR_0330_I2C_CHANNEL				(1)
#define     AR_0330_MODEL_ID_REG_ADDR			(0x00000002)
#define     AR_0330_MODEL_ID_REG_VALUE			(0x2C01)

/* xtal frequency in Khz  */
#define ISS_AR0330_XTAL_KHZ           (10000)

#define PLL_M        49  //pll_multiplier
#define PLL_pre_div  2   //pre_pll_clk_div
#define PLL_P1       1   //vt_sys_clk_div
#define PLL_P2       8   //vt_pix_clk_div


/* color space  */
#define ISS_AR0330_AVI_INFO_COLOR_RGB444       (0)
#define ISS_AR0330_AVI_INFO_COLOR_YUV444       (1)
#define ISS_AR0330_AVI_INFO_COLOR_YUV422       (2)
#define AR0330_ANALOG_GAIN                         0x3060

#define AR0330_COARSE_IT_TIME_A	        0x3012
#define AR0330_ROW_TIME		27      // (2*LINE_LENGTH / OUT_CLK)//27.2us,two paths readout




/*
  Driver handle object
*/
typedef struct
{

    UInt32 state;               /* handle state */

    UInt32 handleId;            /* handle ID, 0..ISS_DEVICE_MAX_HANDLES-1 */

    Semaphore_Handle lock;      /* handle lock */

    Iss_SensorCreateParams createArgs;    /* create time arguments  */

    UInt8 regCache[2][256]; 	/* register read cache */

} Iss_Ar0330Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */

    Semaphore_Handle lock;      /* global driver lock */

    Iss_Ar0330Obj Ar0330Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Ar0330CommonObj;


extern Iss_Ar0330CommonObj gIss_Ar0330CommonObj;

Int32 Iss_Ar0330GetChipId ( Iss_Ar0330Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus );

Int32 Iss_Ar0330Reset ( Iss_Ar0330Obj * pObj );

Int32 Iss_Ar0330Start ( Iss_Ar0330Obj * pObj );

Int32 Iss_Ar0330Stop ( Iss_Ar0330Obj * pObj );

Int32 Iss_Ar0330RegWrite ( Iss_Ar0330Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm );

Int32 Iss_Ar0330RegRead ( Iss_Ar0330Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm );

Int32 Iss_Ar0330LockObj ( Iss_Ar0330Obj * pObj );
Int32 Iss_Ar0330UnlockObj ( Iss_Ar0330Obj * pObj );
Int32 Iss_Ar0330Lock (  );
Int32 Iss_Ar0330Unlock (  );
Iss_Ar0330Obj *Iss_Ar0330AllocObj (  );
Int32 Iss_Ar0330FreeObj ( Iss_Ar0330Obj * pObj );


Int32 Iss_Ar0330PowerDown ( Iss_Ar0330Obj * pObj, UInt32 powerDown );
Int32 Iss_Ar0330OutputEnable ( Iss_Ar0330Obj * pObj, UInt32 enable );

Int32 Iss_Ar0330ResetRegCache(Iss_Ar0330Obj * pObj);
static int Ar0330_GainTableMap(int sensorGain);
Int32 Iss_Ar0330UpdateExpGain ( Iss_Ar0330Obj * pObj, Ptr createArgs );

#define AR0330_GAIN_MAX           41  //maximum index in the gain EVT


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  _ISS_AR0330_PRIV_H_  */
