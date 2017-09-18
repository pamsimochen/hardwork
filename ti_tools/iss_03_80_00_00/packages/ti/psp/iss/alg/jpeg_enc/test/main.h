/** ==================================================================
 *  @file   psp_i2cRdWrUtil.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/i2c/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _GLBCE_TEST_RD_WR_UTIL_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _GLBCE_TEST_RD_WR_UTIL_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/iss_device.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/devices/iss_sensorDriver.h>

#define GLBCE_TEST_TSK_STACK_MAIN      (16*1024)
#define GLBCE_TEST_TSK_PRI_MAIN        (8)

#define GLBCE_TEST_MAX_REG_RD_WR       (0x100)

typedef struct {
    UInt32 i2cInstId;

    char inputStr[1024];
    char *inputArgs[128];
    UInt32 numArgs;

    UInt16 regAddr[GLBCE_TEST_MAX_REG_RD_WR];
    UInt16 regValue[GLBCE_TEST_MAX_REG_RD_WR];

    Task_Handle tskMain;

} GLBCE_TEST_Ctrl;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
