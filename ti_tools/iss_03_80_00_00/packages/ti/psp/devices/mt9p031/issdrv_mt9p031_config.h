/** ==================================================================
 *  @file   issdrv_mt9p031_config.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/mt9p031/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _ISSDRV_MT9P031_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_MT9P031_CONFIG_H_

#define PSP_I2C_MAX_REG_RD_WR       	(0x100)

#define MT9P_031_ADDR					(0x48)             // CCI write
                                                           // address

#define MT9P_031_BOARD_PRECONFIG		(TRUE)             // TURE/FALSE

typedef struct {
    UInt32 i2cInstId;

    UInt32 numArgs;
    UInt16 regAddr[PSP_I2C_MAX_REG_RD_WR];
    UInt16 regValue[PSP_I2C_MAX_REG_RD_WR];
    UInt32 numRegs;
} I2c_Ctrl;

UInt32 SensorConfigScript_Mt9p031[21][3] = {
    {0x000D, 2, 0x0001},
    {0x000D, 2, 0x0000},
    {0x0010, 2, 0x0051},
    {0x0011, 2, 0x1801},
    {0x0012, 2, 0x0002},
    {0x0010, 2, 0x0053},
    {0x0007, 2, 0x1F8E},
    {0x0001, 2, 0x01AF},
    {0x0002, 2, 0x014F},
    {0x0003, 2, 0x0439},
    {0x0004, 2, 0x0781},
    {0x0005, 2, 0x01EC},
    {0x0006, 2, 0x0008},
    {0x0008, 2, 0x0000},
    {0x0009, 2, 0x03AC},
    {0x000C, 2, 0x0000},
    {0x0022, 2, 0x0000},
    {0x0023, 2, 0x0000},
    {0x0035, 2, 0x0016},
    {10, 0, 0}
};
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
