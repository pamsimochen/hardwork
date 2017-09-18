/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
*  \file vpsdrv_hdmi9022a.h
*
*  \brief .
*
*/

#ifndef _VPSDRV_HDMI9022A_H_
#define _VPSDRV_HDMI9022A_H_


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/devices/vps_videoEncoder.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 Vps_hdmi9022aInit(void);
Int32 Vps_hdmi9022aDeInit(void);

Fdrv_Handle Vps_hdmi9022aCreate (UInt32 drvId,
                                 UInt32 instanceId,
                                 Ptr createArgs,
                                 Ptr createStatusArgs,
                                 const FVID2_DrvCbParams * fdmCbParams);

Int32 Vps_hdmi9022aDelete(Fdrv_Handle handle, Ptr deleteArgs);

Int32 Vps_hdmi9022aControl (Fdrv_Handle handle,
                            UInt32 cmd, 
                            Ptr cmdArgs, 
                            Ptr cmdStatusArgs);

#endif
