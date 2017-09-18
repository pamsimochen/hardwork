/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _PSP_I2C_RD_WR_UTIL_H_
#define _PSP_I2C_RD_WR_UTIL_H_


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/vps_device.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/devices/vps_videoDecoder.h>

#define PSP_I2C_TSK_STACK_MAIN      (16*1024)
#define PSP_I2C_TSK_PRI_MAIN        (8)

#define PSP_I2C_MAX_REG_RD_WR       (0x100)

typedef struct
{
    UInt32 i2cInstId;

    char inputStr[1024];
    char *inputArgs[128];
    UInt32 numArgs;

    UInt8 regAddr[PSP_I2C_MAX_REG_RD_WR];
    UInt8 regValue[PSP_I2C_MAX_REG_RD_WR];

    Task_Handle tskMain;

} PspI2c_Ctrl;

#endif
