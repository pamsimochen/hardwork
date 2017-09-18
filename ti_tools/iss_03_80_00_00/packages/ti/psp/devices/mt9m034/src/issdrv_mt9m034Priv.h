/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISS_MT9M034_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_MT9M034_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/mt9m034/issdrv_mt9m034.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

//#define ISS_MT9M034_USE_REG_CACHE
//#define ISS_MT9M034_USE_AVI_FRAME_INFO

/* Driver object state - NOT IN USE */
#define ISS_MT9M034_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_MT9M034_OBJ_STATE_IDLE     (1)

/* MT9M034 I2C Port1 offset from Port0  */
#define ISS_MT9M034_I2C_PORT1_OFFSET   (4)

/* MT9M034 Default TMDS Core ID to use  */
#define ISS_MT9M034_CORE_ID_DEFAULT    (0)



/* MT9M034 Registers - I2C Port 1 */
#define ISS_MT9M034_REG_SYS_PWR_DWN_2   (0x3E)
#define ISS_MT9M034_REG_SYS_PWR_DWN     (0x3F)
#define ISS_MT9M034_REG_AVI_TYPE        (0x40)
#define ISS_MT9M034_REG_AVI_DBYTE15     (0x52)

#define     MT_9M034_I2C_DEV_ADDR				(MT_9M034_ADDR)
#define     MT_9M034_I2C_CHANNEL				(1)
#define     MT_9M034_MODEL_ID_REG_ADDR			(0x00000002)
#define     MT_9M034_MODEL_ID_REG_VALUE			(0x2C01)

/* xtal frequency in Khz  */
#define ISS_MT9M034_XTAL_KHZ           (10000)



/* color space  */
#define ISS_MT9M034_AVI_INFO_COLOR_RGB444       (0)
#define ISS_MT9M034_AVI_INFO_COLOR_YUV444       (1)
#define ISS_MT9M034_AVI_INFO_COLOR_YUV422       (2)


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

} Iss_Mt9m034Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */

    Semaphore_Handle lock;      /* global driver lock */

    Iss_Mt9m034Obj Mt9m034Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Mt9m034CommonObj;


extern Iss_Mt9m034CommonObj gIss_Mt9m034CommonObj;

Int32 Iss_Mt9m034GetChipId ( Iss_Mt9m034Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus );

Int32 Iss_Mt9m034Reset ( Iss_Mt9m034Obj * pObj );

Int32 Iss_Mt9m034Start ( Iss_Mt9m034Obj * pObj );

Int32 Iss_Mt9m034Stop ( Iss_Mt9m034Obj * pObj );

Int32 Iss_Mt9m034RegWrite ( Iss_Mt9m034Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm );

Int32 Iss_Mt9m034RegRead ( Iss_Mt9m034Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm );

Int32 Iss_Mt9m034LockObj ( Iss_Mt9m034Obj * pObj );
Int32 Iss_Mt9m034UnlockObj ( Iss_Mt9m034Obj * pObj );
Int32 Iss_Mt9m034Lock (  );
Int32 Iss_Mt9m034Unlock (  );
Iss_Mt9m034Obj *Iss_Mt9m034AllocObj (  );
Int32 Iss_Mt9m034FreeObj ( Iss_Mt9m034Obj * pObj );


Int32 Iss_Mt9m034PowerDown ( Iss_Mt9m034Obj * pObj, UInt32 powerDown );
Int32 Iss_Mt9m034OutputEnable ( Iss_Mt9m034Obj * pObj, UInt32 enable );

Int32 Iss_Mt9m034ResetRegCache(Iss_Mt9m034Obj * pObj);
static int Mt9m034_GainTableMap(int sensorGain);
Int32 Iss_Mt9m034UpdateExpGain ( Iss_Mt9m034Obj * pObj, Ptr createArgs );
static int Transplant_mt9m034_enable_AE_for_linear_mode(void);
static int Transplant_mt9m034_linear_init_regs(void);
static Uint16 MT9M034_GainTableMap(int aGain)
{
	Uint16 gainRegVal=0;
	
	if(aGain<1000)
	  aGain = 1000;
	
	if(aGain>8000)
	  aGain = 8000;
	
	if(aGain>4000) {
	  aGain = aGain/2;
	  gainRegVal = (1<<6); //2x gain
	}
	
	gainRegVal |= (aGain*8)/1000;
	
	return gainRegVal;

}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  _ISS_MT9M034_PRIV_H_  */
