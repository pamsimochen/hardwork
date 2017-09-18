/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_onChipEncOps.h
 *
 *  \brief Provides interfaces that would required to be implemented by an
 *         on-chip encoders.
 *
 *  Notes:
 *      1. Paried vencs are not aware of the presence of on-chip encoders.
 *      2. On chip encoders are expected to export the operations supported by
 *         them via and vencs defined interface.
 *      3. Flow of control (IOCTL) from apps to on-chip encoders.
 *         Application would call DC control function which intrun calls vencs
 *         control function which in turn call onchip encoders control function.
 *      4. On-Chip encoders are referred to as just encoders / enc in this file
 *
 */

#ifndef _VPSHAL_ONCHIPENCOPS_H
#define _VPSHAL_ONCHIPENCOPS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>

/* ========================================================================== */
/*                              Defaults                                      */
/* ========================================================================== */
#define VPSHAL_ONCHIP_ENC_OPS_DEFAULT = {NULL, NULL, NULL, NULL, NULL}

/* ========================================================================== */
/*                               Operations                                   */
/* ========================================================================== */

/**
 *  getModeEncoder
 *  \brief Get the current mode of the encoder.
 *
 *  \param handle           [IN]    Handle to encoder HAL instance
 *  \param Vps_DcModeInfo   [IN]    Space provided by caller to store mode info
 */
typedef Int (*getModeEncoder) (VpsHal_Handle handle,Vps_DcModeInfo *modeInfo);


/**
 *  setModeEncoder
 *  \brief Update the encoder to operate in specified mode.
 *
 *  \param handle           [IN]    Handle to encoder HAL instance
 *  \param Vps_DcModeInfo   [IN]    New mode of operation
 */
typedef Int (*setModeEncoder) (VpsHal_Handle handle,Vps_DcModeInfo *modeInfo);

/**
 *  startEncoder
 *  \brief Start streaming on the encoder.
 *
 *  \param handle           [IN]    Handle to encoder HAL instance
 *  \param args             [IN]    Arguments if any
 */
typedef Int (*startEncoder) (VpsHal_Handle handle, Ptr args);


/**
 *  stopEncoder
 *  \brief Start streaming on the encoder.
 *
 *  \param handle           [IN]    Handle to encoder HAL instance
 *  \param args             [IN]    Arguments if any
 */
typedef Int (*stopEncoder) (VpsHal_Handle handle, Ptr args);


/**
 *  controlEncoder
 *  \brief Control encoder.
 *
 *  \param handle           [IN]    Handle to encoder HAL instance
 *  \param cmd              [IN]    Command
 *  \param cmdArgs          [IN]    Associated command arguments
 *  \param additionalArgs   [IN]    Additional status if any
 */
typedef Int (*controlEncoder) (   VpsHal_Handle  handle,
                                    UInt32         cmd,
                                    Ptr            cmdArgs,
                                    Ptr            additionalArgs);


/* ========================================================================== */
/*                            Operations Tabel                                */
/* ========================================================================== */

/**
 *  \brief On-Chip Encoders supported operations.
 *
 *         List of operations that could be performed for a given on-chip
 *         encoder.
 */
typedef struct
{
    UInt32          encId;
    /**< Encoders identifier */
    VpsHal_Handle   handle;
    /**< Handle to instance of the encoder */
    getModeEncoder  getModeEnc;
    /**< Get current mode of the encoder */
    setModeEncoder  setModeEnc;
    /**< Update to specified mode of operation */
    startEncoder    startEnc;
    /**< Start streaming on the encoder */
    stopEncoder     stopEnc;
    /**< Stop streaming on the encoder */
    controlEncoder  controlEnc;
    /**< Encoder specific control operations */
}Vps_OnChipEncoderOps;


#ifdef __cplusplus
}
#endif

#endif /* _VPSHAL_HDMI_H */
