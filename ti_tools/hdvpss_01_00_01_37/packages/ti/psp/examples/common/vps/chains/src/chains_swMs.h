/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _CHAINS_SW_MS_H_
#define _CHAINS_SW_MS_H_


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/psp/examples/common/vps/chains/links/swMsLink.h>

#define CHAINS_SW_MS_MAX_DISPLAYS    (3)

Int32 Chains_swMsInit();
Int32 Chains_swMsExit();

Int32 Chains_swMsSwitchLayout(
            UInt32 swMsLinkId[CHAINS_SW_MS_MAX_DISPLAYS],
            SwMsLink_CreateParams swMsPrm[CHAINS_SW_MS_MAX_DISPLAYS],
            Bool switchLayout,
            Bool switchCh,
            UInt32 numDisplay);

Bool Chains_isSwMsAutoSwitchLayout(Bool autoSwitchEnable);
Bool Chains_isSwMsAutoSwitchCh(Bool autoSwitchEnable);

Void Chains_swMsGetLayoutPrm(UInt32 displayNum,
            Bool switchLayout,
            Bool switchCh,
            SwMsLink_LayoutParams *pLayoutPrm);
#endif

