/** ==================================================================
 *  @file   issdrv_ov2710Priv.h
 *                                                                    
 *  @path   /ti/psp/devices/ov2710/src/
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_OV2710_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_OV2710_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/ov2710/issdrv_ov2710.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

/* Driver object state - NOT IN USE */
#define ISS_OV2710_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_OV2710_OBJ_STATE_IDLE     (1)

/* OV2710 I2C Port1 offset from Port0  */
#define ISS_OV2710_I2C_PORT1_OFFSET   (4)

/* OV2710 Default TMDS Core ID to use  */
#define ISS_OV2710_CORE_ID_DEFAULT    (0)

/* OV2710 Registers - I2C Port 0 */
#define ISS_OV2710_REG_VND_IDL         (0x00)
#define ISS_OV2710_REG_VND_IDH         (0x01)
#define ISS_OV2710_REG_DEV_IDL         (0x02)
#define ISS_OV2710_REG_DEV_IDH         (0x03)
#define ISS_OV2710_REG_DEV_REV         (0x04)
#define ISS_OV2710_REG_SW_RST_0        (0x05)
#define ISS_OV2710_REG_STATE           (0x06)
#define ISS_OV2710_REG_SW_RST_1        (0x07)
#define ISS_OV2710_REG_SYS_CTRL_1      (0x08)
#define ISS_OV2710_REG_SYS_SWTCHC      (0x09)
#define ISS_OV2710_REG_H_RESL          (0x3A)
#define ISS_OV2710_REG_H_RESH          (0x3B)
#define ISS_OV2710_REG_V_RESL          (0x3C)
#define ISS_OV2710_REG_V_RESH          (0x3D)
#define ISS_OV2710_REG_VID_CTRL        (0x48)
#define ISS_OV2710_REG_VID_MODE_2      (0x49)
#define ISS_OV2710_REG_VID_MODE_1      (0x4A)
#define ISS_OV2710_REG_VID_BLANK1      (0x4B)
#define ISS_OV2710_REG_VID_BLANK2      (0x4C)
#define ISS_OV2710_REG_VID_BLANK3      (0x4D)
#define ISS_OV2710_REG_DE_PIXL         (0x4E)
#define ISS_OV2710_REG_DE_PIXH         (0x4F)
#define ISS_OV2710_REG_DE_LINL         (0x50)
#define ISS_OV2710_REG_DE_LINH         (0x51)
#define ISS_OV2710_REG_VID_STAT        (0x55)
#define ISS_OV2710_REG_VID_CH_MAP      (0x56)
#define ISS_OV2710_REG_VID_XPCNTL      (0x6E)
#define ISS_OV2710_REG_VID_XPCNTH      (0x6F)

/* OV2710 Registers - I2C Port 1 */
#define ISS_OV2710_REG_SYS_PWR_DWN_2   (0x3E)
#define ISS_OV2710_REG_SYS_PWR_DWN     (0x3F)
#define ISS_OV2710_REG_AVI_TYPE        (0x40)
#define ISS_OV2710_REG_AVI_DBYTE15     (0x52)

#define     OV_2710_I2C_DEV_ADDR				(OV_2710_ADDR)
#define     OV_2710_I2C_CHANNEL				(1)
#define     OV_2710_MODEL_ID_REG_ADDR			(0x00000002)
#define     OV_2710_MODEL_ID_REG_VALUE			(0x2C01)

/* xtal frequency in Khz  */
#define ISS_OV2710_XTAL_KHZ           (10000)



/* color space  */
#define ISS_OV2710_AVI_INFO_COLOR_RGB444       (0)
#define ISS_OV2710_AVI_INFO_COLOR_YUV444       (1)
#define ISS_OV2710_AVI_INFO_COLOR_YUV422       (2)


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

} Iss_Ov2710Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */

    Semaphore_Handle lock;      /* global driver lock */

    Iss_Ov2710Obj Ov2710Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Ov2710CommonObj;

extern Iss_Ov2710CommonObj gIss_Ov2710CommonObj;

/* ===================================================================
 *  @func     Iss_Ov2710GetChipId
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
Int32 Iss_Ov2710GetChipId ( Iss_Ov2710Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus );

/* ===================================================================
 *  @func     Iss_Ov2710Reset
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
Int32 Iss_Ov2710Reset ( Iss_Ov2710Obj * pObj );

/* ===================================================================
 *  @func     Iss_Ov2710Start
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
Int32 Iss_Ov2710Start ( Iss_Ov2710Obj * pObj );

/* ===================================================================
 *  @func     Iss_Ov2710Stop
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
Int32 Iss_Ov2710Stop ( Iss_Ov2710Obj * pObj );

/* ===================================================================
 *  @func     Iss_Ov2710RegWrite
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
Int32 Iss_Ov2710RegWrite ( Iss_Ov2710Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm );

/* ===================================================================
 *  @func     Iss_Ov2710RegRead
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
Int32 Iss_Ov2710RegRead ( Iss_Ov2710Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm );

/* ===================================================================
 *  @func     Iss_Ov2710LockObj
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
Int32 Iss_Ov2710LockObj ( Iss_Ov2710Obj * pObj );

/* ===================================================================
 *  @func     Iss_Ov2710UnlockObj
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
Int32 Iss_Ov2710UnlockObj ( Iss_Ov2710Obj * pObj );

/* ===================================================================
 *  @func     Iss_Ov2710Lock
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
Int32 Iss_Ov2710Lock (  );

/* ===================================================================
 *  @func     Iss_Ov2710Unlock
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
Int32 Iss_Ov2710Unlock (  );

/* ===================================================================
 *  @func     Iss_Ov2710AllocObj
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
Iss_Ov2710Obj *Iss_Ov2710AllocObj (  );

/* ===================================================================
 *  @func     Iss_Ov2710FreeObj
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
Int32 Iss_Ov2710FreeObj ( Iss_Ov2710Obj * pObj );

/* ===================================================================
 *  @func     Iss_Ov2710PowerDown
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
Int32 Iss_Ov2710PowerDown ( Iss_Ov2710Obj * pObj, UInt32 powerDown );

/* ===================================================================
 *  @func     Iss_Ov2710OutputEnable
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
Int32 Iss_Ov2710OutputEnable ( Iss_Ov2710Obj * pObj, UInt32 enable );

/* ===================================================================
 *  @func     Iss_Ov2710ResetRegCache
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
Int32 Iss_Ov2710ResetRegCache(Iss_Ov2710Obj * pObj);

/* ===================================================================
 *  @func     Ov2710_GainTableMap
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
static int Ov2710_GainTableMap(int sensorGain);

/* ===================================================================
 *  @func     Iss_Ov2710UpdateExpGain
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
Int32 Iss_Ov2710UpdateExpGain ( Iss_Ov2710Obj * pObj, Ptr createArgs );

/* ===================================================================
 *  @func     OV2710_GainTableMap
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
static int OV2710_GainTableMap(int sensorGain)
{
	int Again = 0; // default 1x
	if(sensorGain < 1000)
	{
		return Again;
	}
	if(sensorGain < 2000) // 1x to 2x
	{
		Again = 0 + (((sensorGain - 1000) * 16) / 1000);
		return Again;
	}
	if(sensorGain < 4000)  // 2x to 4x
	{
		Again = 0x10 + (((sensorGain - 2000) * 16) / 2000);
		return Again;
	}
	if(sensorGain < 8000)  // 4x to 8x
	{
		Again = 0x30 + (((sensorGain - 4000) * 16) / 4000);
		return Again;
	}
	if(sensorGain < 16000)  // 8x to 16x
	{
		Again = 0x70 + (((sensorGain - 8000) * 16) / 8000);
		return Again;
	}
	if(sensorGain < 32000) // // 16x to 32x
	{
		Again = 0xF0 + (((sensorGain - 16000) * 16) / 16000);
		return Again;
	}
  if(sensorGain >= 32000)
  {
		Again = 0xFF;
		return Again;
  }
	return Again;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  /*  _ISS_OV2710_PRIV_H_  */
