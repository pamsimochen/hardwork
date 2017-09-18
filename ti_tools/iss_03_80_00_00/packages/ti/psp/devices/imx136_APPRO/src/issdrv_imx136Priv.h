/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISS_Imx136_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_Imx136_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/imx136_APPRO/issdrv_imx136.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

/* Driver object state - NOT IN USE */
#define ISS_IMX136_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_IMX136_OBJ_STATE_IDLE     (1)
typedef struct {
	int fps;        // Hz
	int Is50Hz;
	float t_frame;  // ns
	float t_row;    // ns
	float t_pclk;   // ns
	int W;
	int H;
	int SW;
	int SD;
	int HB;
	int VB;
	int HBmin;
	int VBmin;
	int f_pclk;     // Hz

	float shutterOverhead;

	int col_size;
	int row_size;
	int col_skip;
	int row_skip;
	int col_bin;
	int row_bin;
	int col_start;
	int row_start;

	int pll_M;
	int pll_N;
	int pll_DIV;
	int pll_PREDIV;

} Transplant_DRV_imgsFrameTime;

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
} Iss_Imx136Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */
    Semaphore_Handle lock;      /* global driver lock */
    Iss_Imx136Obj Imx136Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Imx136CommonObj;


extern Iss_Imx136CommonObj gIss_Imx136CommonObj;
Int32 Iss_Imx136GetChipId ( Iss_Imx136Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus );
Int32 Iss_Imx136Reset ( Iss_Imx136Obj * pObj );
Int32 Iss_Imx136Start ( Iss_Imx136Obj * pObj );
Int32 Iss_Imx136Stop ( Iss_Imx136Obj * pObj );
Int32 Iss_Imx136RegWrite ( Iss_Imx136Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm );
Int32 Iss_Imx136RegRead ( Iss_Imx136Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm );
Int32 Iss_Imx136LockObj ( Iss_Imx136Obj * pObj );
Int32 Iss_Imx136UnlockObj ( Iss_Imx136Obj * pObj );
Int32 Iss_Imx136Lock (  );
Int32 Iss_Imx136Unlock (  );
Iss_Imx136Obj *Iss_Imx136AllocObj (  );
Int32 Iss_Imx136FreeObj ( Iss_Imx136Obj * pObj );

Int32 Iss_Imx136PowerDown ( Iss_Imx136Obj * pObj, UInt32 powerDown );
Int32 Iss_Imx136OutputEnable ( Iss_Imx136Obj * pObj, UInt32 enable );

Int32 Iss_Imx136UpdateExpGain ( Iss_Imx136Obj * pObj, Ptr createArgs );
int Transplant_DRV_imgsSetEshutter();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  _ISS_Imx136_PRIV_H_  */
