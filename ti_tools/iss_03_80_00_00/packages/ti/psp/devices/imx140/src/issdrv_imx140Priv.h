/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISS_Imx140_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_Imx140_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/imx140/issdrv_imx140.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

/* Driver object state - NOT IN USE */
#define ISS_IMX140_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_IMX140_OBJ_STATE_IDLE     (1)
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
} Iss_Imx140Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */
    Semaphore_Handle lock;      /* global driver lock */
    Iss_Imx140Obj Imx140Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Imx140CommonObj;


extern Iss_Imx140CommonObj gIss_Imx140CommonObj;
Int32 Iss_Imx140GetChipId ( Iss_Imx140Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus );
Int32 Iss_Imx140Reset ( Iss_Imx140Obj * pObj );
Int32 Iss_Imx140Start ( Iss_Imx140Obj * pObj );
Int32 Iss_Imx140Stop ( Iss_Imx140Obj * pObj );
Int32 Iss_Imx140RegWrite ( Iss_Imx140Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm );
Int32 Iss_Imx140RegRead ( Iss_Imx140Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm );
Int32 Iss_Imx140LockObj ( Iss_Imx140Obj * pObj );
Int32 Iss_Imx140UnlockObj ( Iss_Imx140Obj * pObj );
Int32 Iss_Imx140Lock (  );
Int32 Iss_Imx140Unlock (  );
Iss_Imx140Obj *Iss_Imx140AllocObj (  );
Int32 Iss_Imx140FreeObj ( Iss_Imx140Obj * pObj );

Int32 Iss_Imx140PowerDown ( Iss_Imx140Obj * pObj, UInt32 powerDown );
Int32 Iss_Imx140OutputEnable ( Iss_Imx140Obj * pObj, UInt32 enable );

Int32 Iss_Imx140UpdateExpGain ( Iss_Imx140Obj * pObj, Ptr createArgs );
int Imx140_Transplant_DRV_imgsSetEshutter();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  _ISS_Imx140_PRIV_H_  */
