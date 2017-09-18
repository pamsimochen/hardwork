/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_DEVICE_API External Device Interface API
 *
 *  This module defines APIs for external video devices like video
 *  encoders, video decoders, video filters
 *
 *  Typically I2C is used to send control commands to these external devices.
 *  The external device drivers make use of I2C wrapper APIs defined in this module.
 *  The I2C wrapper APIs in turn use the I2C driver to do the actual I2C transfer.
 *
 * @{
*/

/**
 *  \file vps_device.h
 *
 *  \brief External Video Device API
*/

#ifndef _VPS_VIDEO_DEVICE_H_
#define _VPS_VIDEO_DEVICE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name External Video Device Driver ID
 *
 *  Used as drvId when calling FVID2_create()
 *
 */

/* @{ */

/** \brief TVP5158 video decoder driver ID used at the time of FVID2_create() */
#define FVID2_VPS_VID_DEC_TVP5158_DRV    (VPS_VID_DEC_DRV_BASE + 0x0000u)

/** \brief TVP7002 video decoder driver ID used at the time of FVID2_create() */
#define FVID2_VPS_VID_DEC_TVP7002_DRV    (VPS_VID_DEC_DRV_BASE + 0x0001u)

/** \brief HDMI SII9135 video decoder driver ID used at the time of FVID2_create() */
#define FVID2_VPS_VID_DEC_SII9135_DRV    (VPS_VID_DEC_DRV_BASE + 0x0002u)

/** \brief HDMI SII9022a video encoder driver ID used at the time of FVID2_create() */
#define FVID2_VPS_VID_ENC_SII9022A_DRV  (VPS_VID_ENC_DRV_BASE + 0x0000u)

/* @} */

/**
 *  \name I2C instance ID
 *
 *  Used with I2C APIs
 */

/* @{ */

/** \brief I2C instance 0 */
#define VPS_DEVICE_I2C_INST_ID_0   (0)

/** \brief I2C instance 1 */
#define VPS_DEVICE_I2C_INST_ID_1   (1)

/** \brief I2C instance 2 */
#define VPS_DEVICE_I2C_INST_ID_2   (2)

/** \brief I2C instance 3 */
#define VPS_DEVICE_I2C_INST_ID_3   (3)

/** \brief I2C instance not used

    Used as value for Vps_DeviceInitParams.i2cClkKHz[n]
    when it is not needed to init i2c instance 'n'
*/
#define VPS_DEVICE_I2C_INST_NOT_USED   (0xFFFF)

/* @} */

/**
 *  \name Max limits
 */

/* @{ */

/** \brief Max I2C instance's */
#define VPS_DEVICE_I2C_INST_ID_MAX (4)

/** \brief Max handles per external device driver  */
#define VPS_DEVICE_MAX_HANDLES  (4)

/* @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
  \brief External video device sub-system init parameters
*/
typedef struct
{
    Ptr     i2cRegs[VPS_DEVICE_I2C_INST_ID_MAX];
    /**< I2C peripheral base address */

    UInt32  i2cIntNum[VPS_DEVICE_I2C_INST_ID_MAX];
    /**< I2C Interrupt number */

    UInt32  i2cClkKHz[VPS_DEVICE_I2C_INST_ID_MAX];
    /**< I2C bus clock in KHz

      Set to VPS_DEVICE_I2C_INST_NOT_USED in case
      I2C instance init is not needed
    */

    UInt32  isI2cInitReq;
    /**< Indicates whether I2C initialization is required */
} Vps_DeviceInitParams;

/* ========================================================================== */
/*                         Functions                                          */
/* ========================================================================== */

/**
  \brief Initialize external video device subsystem

  - Call this API before creating any external video device driver.
  - Make sure FVID2_init() is called before this API.
  - This API does not do any I2C communication with the external video devices.
  - This API initializes data structures, resources and I2C subsystem needed
    to use the external video devices.

  \param pPrm [IN] init parameters

  \return FVID2_SOK on success, else failure
*/
Int32 Vps_deviceInit ( Vps_DeviceInitParams * pPrm );

/**
  \brief De-Initialize external video device subsystem

  \return FVID2_SOK on success, else failure
*/
Int32 Vps_deviceDeInit (  );

/**
  \brief Initialize I2C device

  \param pPrm [IN] init parameters

  \return FVID2_SOK on success, else failure
*/
Int32 Vps_deviceI2cInit(Vps_DeviceInitParams *pPrm);

/**
  \brief De-Initialize I2C device

  \return FVID2_SOK on success, else failure
*/
Int32 Vps_deviceI2cDeInit(void);

/**
  \brief Read registers from device via I2C

  Make sure size of regAddr[] array and regValue[] array is
  greater than euqal to numRegs.

  The value that gets read from the device is, <br>
  For regAddr[x], value read would be regValue[x]

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1
  \param i2cDevAddr [IN] Slave device address (0 .. 0xFF)
  \param regAddr    [IN] Device Register Address Array (regAddr[x] = 0 .. 0xFF)
  \param regValue   [IN] Device Register Value Array
  \param numRegs    [IN] Number of registers to read ( <= sizeof regAddr[] )

  \return FVID2_SOK on success else failure
*/
Int32 Vps_deviceRead8 ( UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs );

/**
  \brief Raw Read from device via I2C

  Make sure regValue array is
  greater or equal to numRegs.

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1
  \param i2cDevAddr [IN] Slave device address (0 .. 0xFF)
  \param regValue   [IN] Device Register Value Array
  \param numRegs    [IN] Number of Registers to read

  \return FVID2_SOK on success else failure
*/
Int32 Vps_deviceRawRead8 (UInt32 i2cInstId,
                          UInt32 i2cDevAddr,
                          UInt8 *regValue,
                          UInt32 numRegs);

/**
  \brief Write registers to device via I2C

  Make sure size of regAddr array and regValue array is
  greater or equal to numRegs.

  The value that gets written to the device is

  For regAddr[x], value would be regValue[x]

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1
  \param i2cDevAddr [IN] Slave device address (0 .. 0xFF)
  \param regAddr    [IN] Device Register Address Array
  \param regValue   [IN] Device Register Value Array
  \param numRegs    [IN] Number of registers to write

  \return FVID2_SOK on success else failure
*/
Int32 Vps_deviceWrite8 (UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        const UInt8 * regValue,
                        UInt32 numRegs );

/**
  \brief Raw Write to device via I2C

  Make sure size of regValue array is
  greater or equal to numRegs.

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1
  \param i2cDevAddr [IN] Slave device address (0 .. 0xFF)
  \param regValue   [IN] Device Register Value Array
  \param numRegs    [IN] Number of registers to write

  \return FVID2_SOK on success else failure
*/
Int32 Vps_deviceRawWrite8 (UInt32 i2cInstId,
                           UInt32 i2cDevAddr,
                           UInt8 *regValue,
                           UInt32 numRegs);

/**
  \brief Enable/disable debug prints from I2C driver

  Debug prints disabled by default

  \param enable   [IN] TRUE: enable, FALSE: disable

  \return FVID2_SOK on success else failure
*/
Int32 Vps_deviceI2cDebugEnable(UInt32 enable);

/**
  \brief Probes an I2C bus for all video devices.

  Probes an I2C bus for all possibly connected slaves to it.
  Prints the detected slave address on the console.

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1

  \return FVID2_SOK on success else failure
*/
Int32 Vps_deviceI2cProbeAll(UInt16 i2cInstId);

/**
  \brief Probes an I2C bus for a specific device slave address

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1
  \param slaveAddr  [IN] Slave I2C address

  \return FVID2_SOK slave device found, else slave device not found
*/
Int32 Vps_deviceI2cProbeDevice(UInt16 i2cInstId, UInt8 slaveAddr);

#endif /*  _VPS_VIDEO_DEVICE_H_  */

/*@}*/
