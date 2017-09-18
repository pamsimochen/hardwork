/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPSUTILS_H_
#define _VPSUTILS_H_


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/vps.h>


int VpsUtils_getChar(char *pChar, UInt32 timeout);
int VpsUtils_getString(char *pChar, UInt32 timeout);
int VpsUtils_remoteSendChar(char ch);

int VpsUtils_setL3Pri(UInt32 initPressure0, UInt32 initPressure1);

int xstrtoi(char *hex);

#endif
