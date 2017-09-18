/** ==================================================================
 *  @file   issdrv_ar0331Priv.h
 *
 *  @path   /ti/psp/devices/ar0331/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_AR0331_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_AR0331_PRIV_H_

#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/ar0331/issdrv_ar0331.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

// #define ISS_AR0331_USE_REG_CACHE
// #define ISS_AR0331_USE_AVI_FRAME_INFO

/* Driver object state - NOT IN USE */
#define ISS_AR0331_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_AR0331_OBJ_STATE_IDLE     (1)

/* AR0331 I2C Port1 offset from Port0 */
#define ISS_AR0331_I2C_PORT1_OFFSET   (4)

/* AR0331 Default TMDS Core ID to use */
#define ISS_AR0331_CORE_ID_DEFAULT    (0)

/* AR0331 Registers - I2C Port 0 */
#define ISS_AR0331_REG_VND_IDL         (0x00)
#define ISS_AR0331_REG_VND_IDH         (0x01)
#define ISS_AR0331_REG_DEV_IDL         (0x02)
#define ISS_AR0331_REG_DEV_IDH         (0x03)
#define ISS_AR0331_REG_DEV_REV         (0x04)
#define ISS_AR0331_REG_SW_RST_0        (0x05)
#define ISS_AR0331_REG_STATE           (0x06)
#define ISS_AR0331_REG_SW_RST_1        (0x07)
#define ISS_AR0331_REG_SYS_CTRL_1      (0x08)
#define ISS_AR0331_REG_SYS_SWTCHC      (0x09)
#define ISS_AR0331_REG_H_RESL          (0x3A)
#define ISS_AR0331_REG_H_RESH          (0x3B)
#define ISS_AR0331_REG_V_RESL          (0x3C)
#define ISS_AR0331_REG_V_RESH          (0x3D)
#define ISS_AR0331_REG_VID_CTRL        (0x48)
#define ISS_AR0331_REG_VID_MODE_2      (0x49)
#define ISS_AR0331_REG_VID_MODE_1      (0x4A)
#define ISS_AR0331_REG_VID_BLANK1      (0x4B)
#define ISS_AR0331_REG_VID_BLANK2      (0x4C)
#define ISS_AR0331_REG_VID_BLANK3      (0x4D)
#define ISS_AR0331_REG_DE_PIXL         (0x4E)
#define ISS_AR0331_REG_DE_PIXH         (0x4F)
#define ISS_AR0331_REG_DE_LINL         (0x50)
#define ISS_AR0331_REG_DE_LINH         (0x51)
#define ISS_AR0331_REG_VID_STAT        (0x55)
#define ISS_AR0331_REG_VID_CH_MAP      (0x56)
#define ISS_AR0331_REG_VID_XPCNTL      (0x6E)
#define ISS_AR0331_REG_VID_XPCNTH      (0x6F)

/* AR0331 Registers - I2C Port 1 */
#define ISS_AR0331_REG_SYS_PWR_DWN_2   (0x3E)
#define ISS_AR0331_REG_SYS_PWR_DWN     (0x3F)
#define ISS_AR0331_REG_AVI_TYPE        (0x40)
#define ISS_AR0331_REG_AVI_DBYTE15     (0x52)

#define     AR_0331_I2C_DEV_ADDR				(AR_0331_ADDR)
#define     AR_0331_I2C_CHANNEL				(1)
#define     AR_0331_MODEL_ID_REG_ADDR			(0x00000002)
#define     AR_0331_MODEL_ID_REG_VALUE			(0x2C01)

/* xtal frequency in Khz */
#define ISS_AR0331_XTAL_KHZ           (10000)

//#define PLL_M        49                                    // pll_multiplier
//#define PLL_pre_div  2                                     // pre_pll_clk_div
//#define PLL_P1       1                                     // vt_sys_clk_div
//#define PLL_P2       8                                     // vt_pix_clk_div

/* color space */
#define ISS_AR0331_AVI_INFO_COLOR_RGB444       (0)
#define ISS_AR0331_AVI_INFO_COLOR_YUV444       (1)
#define ISS_AR0331_AVI_INFO_COLOR_YUV422       (2)
#define AR0331_DIGITAL_GAIN                         0x305E
#define AR0331_ANALOG_GAIN                         0x3060

#define AR0331_COARSE_IT_TIME_A	        0x3012
//#define ROW_TIME                        29.6297            // 27 //
#define ROW_TIME                        (29.6297) //15.43              // 27
                                                           // (2*LINE_LENGTH
                                                           // /
                                                           // OUT_CLK)//27.2us,two
                                                           // paths readout

/*
 * Driver handle object */
typedef struct {

    UInt32 state;                                          /* handle state */

    UInt32 handleId;                                       /* handle ID,
                                                            * 0..ISS_DEVICE_MAX_HANDLES-1
                                                            */

    Semaphore_Handle lock;                                 /* handle lock */

    Iss_SensorCreateParams createArgs;                     /* create time
                                                            * arguments */

    UInt8 regCache[2][256];                                /* register read
                                                            * cache */

} Iss_Ar0331Obj;

/*
 * Glabal driver object */
typedef struct {
    FVID2_DrvOps fvidDrvOps;                               /* FVID2 driver
                                                            * ops */

    Semaphore_Handle lock;                                 /* global driver
                                                            * lock */

    Iss_Ar0331Obj Ar0331Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Ar0331CommonObj;

extern Iss_Ar0331CommonObj gIss_Ar0331CommonObj;

/* ===================================================================
 *  @func     Iss_Ar0331UpdateItt
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331UpdateItt(Iss_Ar0331Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams);
/* ===================================================================
 *  @func     Iss_Ar0331GetChipId
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331GetChipId(Iss_Ar0331Obj * pObj,
                          Iss_SensorChipIdParams * pPrm,
                          Iss_SensorChipIdStatus * pStatus);

/* ===================================================================
 *  @func     Iss_Ar0331Reset
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331Reset(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Iss_Ar0331Start
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331Start(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Iss_Ar0331Stop
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331Stop(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Iss_Ar0331RegWrite
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331RegWrite(Iss_Ar0331Obj * pObj,
                         Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Ar0331RegRead
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331RegRead(Iss_Ar0331Obj * pObj,
                        Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Ar0331LockObj
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331LockObj(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Iss_Ar0331UnlockObj
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331UnlockObj(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Iss_Ar0331Lock
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331Lock();

/* ===================================================================
 *  @func     Iss_Ar0331Unlock
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331Unlock();

/* ===================================================================
 *  @func     Iss_Ar0331AllocObj
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Iss_Ar0331Obj *Iss_Ar0331AllocObj();

/* ===================================================================
 *  @func     Iss_Ar0331FreeObj
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331FreeObj(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Iss_Ar0331PowerDown
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331PowerDown(Iss_Ar0331Obj * pObj, UInt32 powerDown);

/* ===================================================================
 *  @func     Iss_Ar0331OutputEnable
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331OutputEnable(Iss_Ar0331Obj * pObj, UInt32 enable);

/* ===================================================================
 *  @func     Iss_Ar0331ResetRegCache
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331ResetRegCache(Iss_Ar0331Obj * pObj);

/* ===================================================================
 *  @func     Ar0331_GainTableMap
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static int Ar0331_GainTableMap(int sensorGain);

/* ===================================================================
 *  @func     Iss_Ar0331UpdateExpGain
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331UpdateExpGain(Iss_Ar0331Obj * pObj, Ptr createArgs);

/* ===================================================================
 *  @func     AR0331_GainTableMap
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Uint16 AR0331_GainTableMap(int again)
{
    /*
     * int Again = 0x1040; // default 1x if(sensorGain < 1000) { return
     * Again; } if(sensorGain < 2000) // 1x to 2x { Again = 0x1040 +
     * (((sensorGain - 1000) * 64) / 1000); return Again; } if(sensorGain <
     * 4000) // 2x to 4x { Again = 0x1840 + (((sensorGain - 2000) * 64) /
     * 2000); return Again; } if(sensorGain < 8000) // 4x to 8x { Again =
     * 0x1C40 + (((sensorGain - 4000) * 64) / 4000); return Again; }
     * if(sensorGain < 16000) // 8x to 16x { Again = 0x1CC0 + (((sensorGain -
     * 8000) * 64) / 8000); return Again; } if(sensorGain < 32000) // // 16x
     * to 32x { Again = 0x1DC0 + (((sensorGain - 16000) * 64) / 16000);
     * return Again; } return Again; */

    Uint16 regValue = 0x30;

    if (again >= 8000)
    {
        regValue = 0x30;
    }
    else if (again >= 6400)
    {
        regValue = 0x2C;
    }
    else if (again >= 5333)
    {
        regValue = 0x28;
    }
    else if (again >= 4560)
    {
        regValue = 0x24;
    }
    else if (again >= 4000)
    {
        regValue = 0x20;
    }
    else if (again >= 2666)
    {
        regValue = 0x18 + (((again - 2666) * 3) / 1333)*2;
    }
    else if (again >= 2000)
    {
        regValue = 0x10 + (((again - 2000) * 3) / 666)*2;
    }
    else if (again >= 1333)
    {
        regValue = 0x08 + ((again - 1333) * 7) / 666;
    }
    else
    {
        regValue = 0x00;
    }

    return regValue;

}

/* ===================================================================
 *  @func     Iss_Ar0331FrameRateSet
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_Ar0331FrameRateSet(Iss_Ar0331Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs);

Uint32 Iss_Ar0331PwmVideoSet(Uint32 load_value, Uint32 match_value);
Uint32 Iss_Ar0331PwmDcSet(Uint32 load_value, Uint32 match_value);
Int32 Iss_Ar0331SetMode(Iss_Ar0331Obj *pObj, FVID2_Standard standard, UInt32 devAddr, UInt32 doReset);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_AR0331_PRIV_H_
                                                            */
