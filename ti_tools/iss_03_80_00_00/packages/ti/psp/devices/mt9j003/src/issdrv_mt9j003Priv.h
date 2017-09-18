/** ==================================================================
 *  @file   issdrv_mt9j003Priv.h
 *
 *  @path   /ti/psp/devices/mt9j003/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_MT9J003_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_MT9J003_PRIV_H_

#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/mt9j003/issdrv_mt9j003.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

// #define ISS_MT9J003_USE_REG_CACHE
// #define ISS_MT9J003_USE_AVI_FRAME_INFO

/* Driver object state - NOT IN USE */
#define ISS_MT9J003_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_MT9J003_OBJ_STATE_IDLE     (1)

/* MT9J003 I2C Port1 offset from Port0 */
#define ISS_MT9J003_I2C_PORT1_OFFSET   (4)

/* MT9J003 Default TMDS Core ID to use */
#define ISS_MT9J003_CORE_ID_DEFAULT    (0)

/* MT9J003 Registers - I2C Port 0 */
#define ISS_MT9J003_REG_VND_IDL         (0x00)
#define ISS_MT9J003_REG_VND_IDH         (0x01)
#define ISS_MT9J003_REG_DEV_IDL         (0x02)
#define ISS_MT9J003_REG_DEV_IDH         (0x03)
#define ISS_MT9J003_REG_DEV_REV         (0x04)
#define ISS_MT9J003_REG_SW_RST_0        (0x05)
#define ISS_MT9J003_REG_STATE           (0x06)
#define ISS_MT9J003_REG_SW_RST_1        (0x07)
#define ISS_MT9J003_REG_SYS_CTRL_1      (0x08)
#define ISS_MT9J003_REG_SYS_SWTCHC      (0x09)
#define ISS_MT9J003_REG_H_RESL          (0x3A)
#define ISS_MT9J003_REG_H_RESH          (0x3B)
#define ISS_MT9J003_REG_V_RESL          (0x3C)
#define ISS_MT9J003_REG_V_RESH          (0x3D)
#define ISS_MT9J003_REG_VID_CTRL        (0x48)
#define ISS_MT9J003_REG_VID_MODE_2      (0x49)
#define ISS_MT9J003_REG_VID_MODE_1      (0x4A)
#define ISS_MT9J003_REG_VID_BLANK1      (0x4B)
#define ISS_MT9J003_REG_VID_BLANK2      (0x4C)
#define ISS_MT9J003_REG_VID_BLANK3      (0x4D)
#define ISS_MT9J003_REG_DE_PIXL         (0x4E)
#define ISS_MT9J003_REG_DE_PIXH         (0x4F)
#define ISS_MT9J003_REG_DE_LINL         (0x50)
#define ISS_MT9J003_REG_DE_LINH         (0x51)
#define ISS_MT9J003_REG_VID_STAT        (0x55)
#define ISS_MT9J003_REG_VID_CH_MAP      (0x56)
#define ISS_MT9J003_REG_VID_XPCNTL      (0x6E)
#define ISS_MT9J003_REG_VID_XPCNTH      (0x6F)

/* MT9J003 Registers - I2C Port 1 */
#define ISS_MT9J003_REG_SYS_PWR_DWN_2   (0x3E)
#define ISS_MT9J003_REG_SYS_PWR_DWN     (0x3F)
#define ISS_MT9J003_REG_AVI_TYPE        (0x40)
#define ISS_MT9J003_REG_AVI_DBYTE15     (0x52)

#define     MT9J_003_I2C_DEV_ADDR               (MT9J_003_ADDR)
#define     MT9J_003_I2C_CHANNEL                (1)
#define     MT9J_003_MODEL_ID_REG_ADDR          (0x00000002)
#define     MT9J_003_MODEL_ID_REG_VALUE         (0x2C01)

/* xtal frequency in Khz */
#define ISS_MT9J003_XTAL_KHZ           (10000)

/* color space */
#define ISS_MT9J003_AVI_INFO_COLOR_RGB444       (0)
#define ISS_MT9J003_AVI_INFO_COLOR_YUV444       (1)
#define ISS_MT9J003_AVI_INFO_COLOR_YUV422       (2)

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

} Iss_Mt9j003Obj;

/*
 * Glabal driver object */
typedef struct {
    FVID2_DrvOps fvidDrvOps;                               /* FVID2 driver
                                                            * ops */

    Semaphore_Handle lock;                                 /* global driver
                                                            * lock */

    Iss_Mt9j003Obj Mt9j003Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Mt9j003CommonObj;

extern Iss_Mt9j003CommonObj gIss_Mt9j003CommonObj;

/* ===================================================================
 *  @func     Iss_Mt9j003GetChipId
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
Int32 Iss_Mt9j003GetChipId(Iss_Mt9j003Obj * pObj,
                           Iss_SensorChipIdParams * pPrm,
                           Iss_SensorChipIdStatus * pStatus);

/* ===================================================================
 *  @func     Iss_Mt9j003Reset
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
Int32 Iss_Mt9j003Reset(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mt9j003Start
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
Int32 Iss_Mt9j003Start(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mt9j003Stop
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
Int32 Iss_Mt9j003Stop(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mt9j003RegWrite
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
Int32 Iss_Mt9j003RegWrite(Iss_Mt9j003Obj * pObj,
                          Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Mt9j003RegRead
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
Int32 Iss_Mt9j003RegRead(Iss_Mt9j003Obj * pObj,
                         Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Mt9j003LockObj
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
Int32 Iss_Mt9j003LockObj(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mt9j003UnlockObj
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
Int32 Iss_Mt9j003UnlockObj(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mt9j003Lock
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
Int32 Iss_Mt9j003Lock();

/* ===================================================================
 *  @func     Iss_Mt9j003Unlock
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
Int32 Iss_Mt9j003Unlock();

/* ===================================================================
 *  @func     Iss_Mt9j003AllocObj
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
Iss_Mt9j003Obj *Iss_Mt9j003AllocObj();

/* ===================================================================
 *  @func     Iss_Mt9j003FreeObj
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
Int32 Iss_Mt9j003FreeObj(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mt9j003PowerDown
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
Int32 Iss_Mt9j003PowerDown(Iss_Mt9j003Obj * pObj, UInt32 powerDown);

/* ===================================================================
 *  @func     Iss_Mt9j003OutputEnable
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
Int32 Iss_Mt9j003OutputEnable(Iss_Mt9j003Obj * pObj, UInt32 enable);

/* ===================================================================
 *  @func     Iss_Mt9j003ResetRegCache
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
Int32 Iss_Mt9j003ResetRegCache(Iss_Mt9j003Obj * pObj);

/* ===================================================================
 *  @func     MT9J003_GainTableMap
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
static int MT9J003_GainTableMap(int sensorGain);

/* ===================================================================
 *  @func     Iss_Mt9j003UpdateExpGain
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
Int32 Iss_Mt9j003UpdateExpGain(Iss_Mt9j003Obj * pObj, Ptr createArgs);

/* ===================================================================
 *  @func     Iss_Mt9j003UpdateItt
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
Int32 Iss_Mt9j003UpdateItt(Iss_Mt9j003Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams);
/* ===================================================================
 *  @func     Iss_Mt9j003UpdateFrameRate
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
Int32 Iss_Mt9j003UpdateFrameRate(Iss_Mt9j003Obj * pObj,
                                 Iss_CaptFrameRate * framerateParams);
/* ===================================================================
 *  @func     Iss_Mt9j003FrameRateSet
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
Int32 Iss_Mt9j003FrameRateSet(Iss_Mt9j003Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs);
Int32 Iss_mt9t003SetMode(Iss_Mt9j003Obj *pObj, FVID2_Standard standard, UInt32 devAddr, UInt32 doReset);
/* ===================================================================
 *  @func     MT9J003_GainTableMap
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
static int MT9J003_GainTableMap(int sensorGain)
{
    int Again = 0x1040;                                    // default 1x

    if (sensorGain < 1000)
    {
        return Again;
    }
    if (sensorGain < 2000)                                 // 1x to 2x
    {
        Again = 0x1040 + (((sensorGain - 1000) * 64) / 1000);
        return Again;
    }
    if (sensorGain < 4000)                                 // 2x to 4x
    {
        Again = 0x1840 + (((sensorGain - 2000) * 64) / 2000);
        return Again;
    }
    if (sensorGain < 8000)                                 // 4x to 8x
    {
        Again = 0x1C40 + (((sensorGain - 4000) * 64) / 4000);
        return Again;
    }
    if (sensorGain < 16000)                                // 8x to 16x
    {
        Again = 0x1CC0 + (((sensorGain - 8000) * 64) / 8000);
        return Again;
    }
    if(sensorGain < 32000) // // 16x to 32x
    {
        Again = 0x1DC0 + (((sensorGain - 16000) * 64) / 16000);
        return Again;
    }
  if(sensorGain >= 32000)
  {
        Again = 0x1DFF;
        return Again;
  }
    return Again;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_MT9J003_PRIV_H_
                                                            */
