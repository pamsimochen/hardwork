/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPSDRV_SII9135_H_
#define _VPSDRV_SII9135_H_

#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

Int32 Vps_sii9135Init (  );
Int32 Vps_sii9135DeInit (  );

Fdrv_Handle Vps_sii9135Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams );

Int32 Vps_sii9135Delete ( Fdrv_Handle handle, Ptr deleteArgs );

Int32 Vps_sii9135Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif
