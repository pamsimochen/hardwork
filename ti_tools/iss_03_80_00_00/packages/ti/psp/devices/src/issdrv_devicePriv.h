/** ==================================================================
 *  @file   issdrv_devicePriv.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISSDRV_DEVICE_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_DEVICE_PRIV_H_

#include <xdc/runtime/System.h>
#include <ti/psp/devices/iss_device.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/i2c/psp_i2c.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <string.h>

#define ISS_DEVICE_I2C_TIMEOUT      (BIOS_WAIT_FOREVER)
/* 
 * typedef struct { UInt32 i2cDebugEnable;
 * 
 * PSP_I2cHandle i2cHndl[ISS_DEVICE_I2C_INST_ID_MAX]; Semaphore_Handle
 * i2cHndlLock[ISS_DEVICE_I2C_INST_ID_MAX];
 * 
 * } Iss_DeviceObj;
 * 
 * extern Vps_DeviceObj gIss_deviceObj; */
extern Vps_DeviceObj gVps_deviceObj;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISSDRV_DEVICE_PRIV_H_ 
                                                            */
