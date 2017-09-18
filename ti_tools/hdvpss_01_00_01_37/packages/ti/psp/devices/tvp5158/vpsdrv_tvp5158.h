/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPSDRV_TVP5158_H_
#define _VPSDRV_TVP5158_H_

#include <ti/psp/devices/vps_tvp5158.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

Int32 Vps_tvp5158Init (  );
Int32 Vps_tvp5158DeInit (  );

Fdrv_Handle Vps_tvp5158Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams );

Int32 Vps_tvp5158Delete ( Fdrv_Handle handle, Ptr deleteArgs );

Int32 Vps_tvp5158Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif
