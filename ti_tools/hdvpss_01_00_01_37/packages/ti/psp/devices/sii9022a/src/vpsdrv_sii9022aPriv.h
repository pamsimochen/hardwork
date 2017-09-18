/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_hdmi9022aPriv.h
 *
 *  \brief HDMI9022a driver
 *  This file implements functionality for the HDMI.
 *
 */

#ifndef _VPS_HDMI9022A_PRIV_H_
#define _VPS_HDMI9022A_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/vps_videoEncoder.h>
#include <ti/psp/devices/vps_sii9022a.h>
#include <ti/psp/devices/sii9022a/vpsdrv_sii9022a.h>
#include <ti/psp/devices/sii9022a/src/vpsdrv_sii9022aPriv.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <xdc/runtime/System.h>
#include <string.h>
#include <xdc/runtime/System.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief SII9022A handle state - not used. */
#define VPS_SII9022A_OBJ_STATE_UNUSED   (0)

/** \brief SII9022A handle state -  used */
#define VPS_SII9022A_OBJ_STATE_USED     (1)

/** \brief SII9022A device state -  initilized succefully */
#define VPS_SII9022A_NOT_INITED         (0)

/** \brief SII9022A device state -  initilized succefully */
#define VPS_SII9022A_INITED             (1)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief HDMI Object.
 */
typedef struct
{
    UInt32 state;
    /**< State of HDMI */
    UInt32 handleId;
    /**< handle ID, 0..VPS_DEVICE_MAX_HANDLES-1  */
    Semaphore_Handle lock;
    /** < handle lock */
    Vps_VideoEncoderCreateParams createArgs;
    /* create time arguments */
    Vps_HdmiChipId  hdmiChipid;
    /* HDMI Chip id */
    UInt32  hpd;
    /* HDMI Hot plug detect */
    UInt32 syncCfgReg;
    /**< Sync Configuration Register */
    UInt32 syncPolarityReg;
    UInt32 isRgbOutput;
    UInt32 inBusCfg;
} Vps_Sii9022aObj;



/*
  Overall HDMI driver object
*/
typedef struct
{
    UInt32 deviceState;
    /**< FVID2 driver ops */
    FVID2_DrvOps fvidDrvOps;
    /**< FVID2 driver ops */
    Semaphore_Handle glock;
    /**<global level lock */
    Vps_Sii9022aObj sii9022aObj[VPS_DEVICE_MAX_HANDLES];
    /** < HDMI handle objects */
    Vps_SiI9022aPrms prms;
    /**< 9022A params */
} Vps_Sii9022aCommonObj;

#endif

