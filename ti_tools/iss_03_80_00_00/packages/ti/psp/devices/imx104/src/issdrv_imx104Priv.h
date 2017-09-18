/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISS_Imx104_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_Imx104_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/imx104/issdrv_imx104.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

#include <math.h>

/* Driver object state - NOT IN USE */
#define ISS_IMX104_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_IMX104_OBJ_STATE_IDLE     (1)

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

} Iss_Imx104Obj;

/*
  Glabal driver object
*/
typedef struct
{
    FVID2_DrvOps fvidDrvOps;    /* FVID2 driver ops */

    Semaphore_Handle lock;      /* global driver lock */

    Iss_Imx104Obj Imx104Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Imx104CommonObj;


extern Iss_Imx104CommonObj gIss_Imx104CommonObj;

Int32 Iss_Imx104GetChipId ( Iss_Imx104Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus );

Int32 Iss_Imx104Reset ( Iss_Imx104Obj * pObj );

Int32 Iss_Imx104Start ( Iss_Imx104Obj * pObj );

Int32 Iss_Imx104Stop ( Iss_Imx104Obj * pObj );

Int32 Iss_Imx104RegWrite ( Iss_Imx104Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm );

Int32 Iss_Imx104RegRead ( Iss_Imx104Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm );

Int32 Iss_Imx104LockObj ( Iss_Imx104Obj * pObj );
Int32 Iss_Imx104UnlockObj ( Iss_Imx104Obj * pObj );
Int32 Iss_Imx104Lock (  );
Int32 Iss_Imx104Unlock (  );
Iss_Imx104Obj *Iss_Imx104AllocObj (  );
Int32 Iss_Imx104FreeObj ( Iss_Imx104Obj * pObj );


Int32 Iss_Imx104PowerDown ( Iss_Imx104Obj * pObj, UInt32 powerDown );
Int32 Iss_Imx104OutputEnable ( Iss_Imx104Obj * pObj, UInt32 enable );

Int32 Iss_Imx104UpdateExpGain ( Iss_Imx104Obj * pObj, Ptr createArgs );
int Transplant_DRV_imgsSetEshutter();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  _ISS_Imx104_PRIV_H_  */
