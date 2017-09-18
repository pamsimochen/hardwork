/** ==================================================================
 *  @file   iss_device.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \defgroup ISS_DRV_FVID2_DEVICE_API External Device Interface API
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
 *  \file iss_device.h
 *
 *  \brief External Video Device API
*/

#ifndef _ISS_VIDEO_DEVICE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_VIDEO_DEVICE_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/iss/iss.h>
#include <ti/psp/devices/vps_device.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

/**
 *  \name External Video Device Driver ID
 *
 *  Used as drvId when calling FVID2_create()
 *
 */

/* @{ */
#define FVID2_ISS_SENSOR_MT9J003_DRV	(ISS_SENSOR_DRV_BASE + 0x0000u)
#define FVID2_ISS_SENSOR_AR0331_DRV		(ISS_SENSOR_DRV_BASE + 0x0001u)
#define FVID2_ISS_SENSOR_MN34041_DRV	(ISS_SENSOR_DRV_BASE + 0x0002u)
#define FVID2_ISS_SENSOR_IMX035_DRV		(ISS_SENSOR_DRV_BASE + 0x0003u)
#define FVID2_ISS_SENSOR_OV2715_DRV		(ISS_SENSOR_DRV_BASE + 0x0004u)
#define FVID2_ISS_SENSOR_IMX036_DRV		(ISS_SENSOR_DRV_BASE + 0x0005u)
#define FVID2_ISS_SENSOR_OV9712_DRV		(ISS_SENSOR_DRV_BASE + 0x0006u)
#define FVID2_ISS_SENSOR_OV10630_DRV	(ISS_SENSOR_DRV_BASE + 0x0007u)
#define FVID2_ISS_SENSOR_MT9P031_DRV	(ISS_SENSOR_DRV_BASE + 0x0008u)
#define FVID2_ISS_SENSOR_MT9D131_DRV	(ISS_SENSOR_DRV_BASE + 0x0009u)
#define FVID2_ISS_SENSOR_MT9M034_DRV	(ISS_SENSOR_DRV_BASE + 0x000au)
#define FVID2_ISS_SENSOR_TVP514X_DRV	(ISS_SENSOR_DRV_BASE + 0x000bu)
#define FVID2_ISS_SENSOR_OV7740_DRV		(ISS_SENSOR_DRV_BASE + 0x000cu)
#define FVID2_ISS_SENSOR_MT9M034_DUAL_HEAD_BOARD_DRV (ISS_SENSOR_DRV_BASE + 0x000Du)
#define FVID2_ISS_SENSOR_IMX136_DRV		(ISS_SENSOR_DRV_BASE + 0x000Eu)
#define FVID2_ISS_SENSOR_AR0330_DRV		(ISS_SENSOR_DRV_BASE + 0x000Fu)
#define FVID2_ISS_SENSOR_IMX104_DRV		(ISS_SENSOR_DRV_BASE + 0x0010u)
#define FVID2_ISS_SENSOR_AL30210_DRV	(ISS_SENSOR_DRV_BASE + 0x0011u)
#define FVID2_ISS_SENSOR_OV2710_DRV		(ISS_SENSOR_DRV_BASE + 0x0012u)
#define FVID2_ISS_SENSOR_IMX122_DRV		(ISS_SENSOR_DRV_BASE + 0x0013u)		/* Sony IMX-122 Sensor */
#define FVID2_ISS_SENSOR_IMX140_DRV		(ISS_SENSOR_DRV_BASE + 0x0014u)		/* Sony IMX-140 Sensor */


//#ifdef DM385

#define CTRL_MODULE_BASE_ADDR   (0x48140000u)
#define GPIO_MODULE_BASE_ADDR   (0x4804c000u)
#define PINCNTL153				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A60) //for CAM_RESET


#define	PINCNTL156				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A6C)
#define	PINCNTL157				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A70)
#define	PINCNTL158				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A74)
#define	PINCNTL159				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A78)
#define	PINCNTL160				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A7C)
#define	PINCNTL161				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A80)
#define	PINCNTL162				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A84)
#define	PINCNTL163				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A88)
#define	PINCNTL164				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A8C)
#define	PINCNTL165				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A90)
#define	PINCNTL166				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A94)
#define	PINCNTL167				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A98)
#define	PINCNTL168				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A9C)
#define	PINCNTL169				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AA0)
#define	PINCNTL170				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AA4)
#define	PINCNTL171				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AA8)
#define	PINCNTL172				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AAC)
#define	PINCNTL173				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB0)
#define	PINCNTL174				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB4)
#define	PINCNTL175				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB8)

#define PINCNTL74				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0924)
#define PINCNTL75				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0928)

#define	PINCNTL135				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A18)
#define	PINCNTL136				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A1C)

#define PINCNTL85				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0950)
#define PINCNTL86				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0954)
#define PINCNTL87				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0958)//no used
#define PINCNTL88				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x095C)

#define GIO_INPUT_OUTPUT_DIR                    (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0134)
#define GIO_ENABLE_DISABLE_WAKEUP               (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0044)

#ifndef CBB_PLATFORM
#define GIO_WRITE_DATA                          (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0194)
#define GIO_CLEAR_DATA                          (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0190)
#endif

/* @} */

/**
 *  \name I2C instance ID
 *
 *  Used with I2C APIs
 */

/* @{ */

/** \brief I2C instance 0 */
#define ISS_DEVICE_I2C_INST_ID_0   (0)

/** \brief I2C instance 1 */
#define ISS_DEVICE_I2C_INST_ID_1   (1)

/** \brief I2C instance 2 */
#define ISS_DEVICE_I2C_INST_ID_2   (2)

/** \brief I2C instance 3 */
#define ISS_DEVICE_I2C_INST_ID_3   (3)

/** \brief I2C instance not used

    Used as value for Iss_DeviceInitParams.i2cClkKHz[n]
    when it is not needed to init i2c instance 'n'
*/
#define ISS_DEVICE_I2C_INST_NOT_USED   (0xFFFF)

/* @} */

/**
 *  \name Max limits
 */

/* @{ */

/** \brief Max I2C instance's */
#define ISS_DEVICE_I2C_INST_ID_MAX (4)

/** \brief Max handles per external device driver  */
#define ISS_DEVICE_MAX_HANDLES  (4)

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
#define Iss_deviceRead8 Vps_deviceRead8

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
#define Iss_deviceRawRead8 Vps_deviceRawRead8

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
#define Iss_deviceWrite8 Vps_deviceWrite8

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
#define Iss_deviceRawWrite8 Vps_deviceRawWrite8

/**
  \brief Enable/disable debug prints from I2C driver

  Debug prints disabled by default

  \param enable   [IN] TRUE: enable, FALSE: disable

  \return FVID2_SOK on success else failure
*/
#define Iss_deviceI2cDebugEnable Vps_deviceI2cDebugEnable

#define Iss_deviceI2cLock Vps_deviceI2cLock

/**
  \brief Initialize I2C device

  \param pPrm [IN] init parameters

  \return FVID2_SOK on success, else failure
*/
#define Iss_deviceI2cInit Vps_deviceI2cInit

/**
  \brief De-Initialize I2C device

  \return FVID2_SOK on success, else failure
*/
#define Iss_deviceI2cDeInit Vps_deviceI2cDeInit

/**
  \brief Probes an I2C bus for all video devices.

  Probes an I2C bus for all possibly connected slaves to it.
  Prints the detected slave address on the console.

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1

  \return FVID2_SOK on success else failure
*/
#define Iss_deviceI2cProbeAll Vps_deviceI2cProbeAll

/**
  \brief Probes an I2C bus for a specific device slave address

  \param i2cInstId  [IN] \ref VPS_DEVICE_I2C_INST_ID_0 or \ref VPS_DEVICE_I2C_INST_ID_1
  \param slaveAddr  [IN] Slave I2C address

  \return FVID2_SOK slave device found, else slave device not found
*/
#define Iss_deviceI2cProbeDevice Vps_deviceI2cProbeDevice

/* @} */

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/**
  \brief External video device sub-system init parameters
*/
typedef struct {
    Ptr i2cRegs[ISS_DEVICE_I2C_INST_ID_MAX];
    /**< I2C peripheral base address */

    UInt32 i2cIntNum[ISS_DEVICE_I2C_INST_ID_MAX];
    /**< I2C Interrupt number */

    UInt32 i2cClkKHz[ISS_DEVICE_I2C_INST_ID_MAX];
    /**< I2C bus clock in KHz

      Set to ISS_DEVICE_I2C_INST_NOT_USED in case
      I2C instance init is not needed
    */

    UInt32 isI2cInitReq;
    /**< Indicates whether I2C initialization is required */
} Iss_DeviceInitParams;

/* ========================================================================== 
 */
/* Functions */
/* ========================================================================== 
 */

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
/* ===================================================================
 *  @func     Iss_deviceInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_deviceInit(Vps_DeviceInitParams * pPrm);

/**
  \brief De-Initialize external video device subsystem

  \return FVID2_SOK on success, else failure
*/
/* ===================================================================
 *  @func     Iss_deviceDeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_deviceDeInit();

/**
  \brief Read registers from device via I2C

  Make sure size of regAddr[] array and regValue[] array is
  greater than euqal to numRegs.

  The value that gets read from the device is, <br>
  For regAddr[x], value read would be regValue[x]

  \param i2cInstId  [IN] \ref ISS_DEVICE_I2C_INST_ID_0 or \ref ISS_DEVICE_I2C_INST_ID_1
  \param i2cDevAddr [IN] Slave device address (0 .. 0xFF)
  \param regAddr    [IN] Device Register Address Array (regAddr[x] = 0 .. 0xFF)
  \param regValue   [IN] Device Register Value Array
  \param numRegs    [IN] Number of registers to read ( <= sizeof regAddr[] )

  \return FVID2_SOK on success else failure
*/

Int32 Iss_deviceRead16(UInt32 i2cInstId, UInt32 i2cDevAddr,
                       const UInt16 * regAddr,
                       UInt16 * regValue, UInt32 numRegs);

/**
  \brief Write registers to device via I2C

  Make sure size of regAddr array and regValue array is
  greater or equal to numRegs.

  The value that gets written to the device is

  For regAddr[x], value would be regValue[x]

  \param i2cInstId  [IN] \ref ISS_DEVICE_I2C_INST_ID_0 or \ref ISS_DEVICE_I2C_INST_ID_1
  \param i2cDevAddr [IN] Slave device address (0 .. 0xFF)
  \param regAddr    [IN] Device Register Address Array
  \param regValue   [IN] Device Register Value Array
  \param numRegs    [IN] Number of registers to write

  \return FVID2_SOK on success else failure
*/

Int32 Iss_deviceWrite16(UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt16 * regAddr,
                        const UInt16 * regValue, UInt32 numRegs);

#ifdef CBB_PLATFORM
Int32 iss_gpio_write_data(UInt32 band, UInt32 io_no, UInt32 data);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_VIDEO_DEVICE_H_ 
                                                            */

/* @} */
