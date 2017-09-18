/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISSDRV_Imx136_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_Imx136_H_

#include <ti/psp/devices/iss_sensorDriver.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>



Int32 Iss_Imx136Init (  );

Fdrv_Handle Iss_Imx136Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams );


Int32 Iss_Imx136Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );
                           
Int32 Iss_Imx136Delete ( Fdrv_Handle handle, Ptr deleteArgs );                       
                           
Int32 Iss_Imx136DeInit (  );

Int32 Iss_Imx136PinMux (  );

void imx136_setgio(unsigned short addr, unsigned short reg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

