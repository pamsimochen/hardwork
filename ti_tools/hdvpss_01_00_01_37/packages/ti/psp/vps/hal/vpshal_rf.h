/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_rf.h
 *
 * \brief VPS RF Modulator HAL header file
 * This file exposes the HAL APIs of the VPS RF Modulator.
 *
 */

#ifndef _VPSHAL_RF_H
#define _VPSHAL_RF_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/vps.h>

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief RF Modulator instance connected to the SD Venc */
#define VPSHAL_RF_INST_SD_VENC          (0u)
/** \brief RF maximum number of instances */
#define VPSHAL_RF_MAX_INST              (1u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_RfInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 *
 */
typedef struct
{
    UInt32             instId;
    /**< RF Instance */
    UInt32             baseAddress;
    /**< Base address of the instance */
    UInt32      vencId;
    /**< Paired venc ID, as defined by Display Controller */
    UInt32      encoderId;
    /**< Identifer assigned to this instace by Display Controller */
} VpsHal_RfInstParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief RF HAL init function.
 *
 *  Initializes RF objects.
 *  This function should be called before calling any of RF HAL APIs.
 *  SD Venc HAL should be initialized prior to calling any of RF HAL
 *  functions.
 *  All global variables are initialized.
 *
 *  \param numInstances  Number of RF Modulator Instances and number of
 *                       array elements in the second argument.
 *  \param initParams    Instance Specific Parameters
 *  \param arg           Not used currently. Meant for future purpose.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_rfInit(UInt32 numInstances,
                   const VpsHal_RfInstParams *initParams,
                   Ptr arg);

/**
 *  \brief RF HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg     Not used currently. Meant for future purpose.
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_rfDeInit(Ptr arg);


/**
 *  \brief Returns the handle to the requested RF instance.
 *
 *  This function should be called prior to calling any of the RF HAL
 *  configuration APIs to get the instance handle.
 *  VpsHal_rfInit should be called prior to this.
 *
 *  \param rfInst   Requested RF instance.
 *
 *  \return         Returns RF instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_rfOpen(UInt32 rfInst);

/**
 *  \brief Closes the RF HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case in
 *  future, resource management is done by individual HALs - using counters.
 *  VpsHal_rfInit and VpsHal_rfOpen should be called prior to this.
 *
 *  \param handle   Valid handle returned by VpsHal_rfOpen function.
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_rfClose(VpsHal_Handle handle);


/**
 *  \brief Switches ON the RF DAC.
 *
 *         VpsHal_rfStart is expected to be called after:
 *         //TBD
 *
 *  \param handle   [IN]    Handle to instance of RF Hal.
 *  \param args     [IN]    Not used for now, reserved for future expansion.
 *                          Should be NULL for now.
 */
Int32 VpsHal_rfStart(VpsHal_Handle handle,
                    Ptr args);


/**
 *  \brief Switches OFF the RF DAC.
 *
 *  \param handle   [IN]    Handle to instance of RF Hal.
 *  \param args     [IN]    Not used for now, reserved for future expansion.
 *                          Should be NULL for now.
 */
Int32 VpsHal_rfStop (VpsHal_Handle handle,
                    Ptr args);


/**
 *  \brief Used to updated RF non-generic configurations, notifications, among
 *         others.
 *
 *         The HDMI Peripheral has capabilities to convert the color space being
 *         sent out, dither the output, etc... #Vps_HdmiConfigParams list the
 *         configureable options available.
 *         Could also be used to notify apps on dectection of a sink, read sinks
 *         EDID blocks, etc...
 *
 *  \param handle   [IN]    Handle to instance of RF HAL
 *  \param cmd      [IN]    Command - Supported commands are...
 *                          #IOCTL_VPS_DCTRL_HDMI_GET_CONFIG
 *                          #IOCTL_VPS_DCTRL_HDMI_SET_CONFIG
 *  \param cmdArgs  [IN]    Depends on the command. Please refer the command
 *  \param additionalArgs [IN] Depends on the command. Please refer the command
 */
Int32 VpsHal_rfControl(VpsHal_Handle handle,
                        UInt32 cmd,
                        Ptr cmdArgs,
                        Ptr additionalArgs);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_RF_H */

