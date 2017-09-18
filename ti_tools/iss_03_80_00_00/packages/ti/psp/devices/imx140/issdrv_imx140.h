/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISSDRV_Imx140_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_Imx140_H_

#include <ti/psp/devices/iss_sensorDriver.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>



Int32 Iss_Imx140Init (  );

Fdrv_Handle Iss_Imx140Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams );


Int32 Iss_Imx140Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );
                           
Int32 Iss_Imx140Delete ( Fdrv_Handle handle, Ptr deleteArgs );                       
                           
Int32 Iss_Imx140DeInit (  );

Int32 Iss_Imx140PinMux (  );

void imx140_setgio(unsigned short addr, unsigned short reg);

Uint8 imx140_getgio(Uint16 regaddr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

