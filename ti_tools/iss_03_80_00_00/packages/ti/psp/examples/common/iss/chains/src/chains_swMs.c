/** ==================================================================
 *  @file   chains_swMs.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "chains_swMs.h"
#include <ti/sysbios/knl/Clock.h>

#define CHAINS_SW_MS_AUTO_LAYOUT_SWITCH_INTERVAL_IN_SECS    (5)
#define CHAINS_SW_MS_AUTO_CH_SWITCH_INTERVAL_IN_SECS        (100000)

typedef struct {
    Clock_Handle timer;

    UInt32 layoutId[CHAINS_SW_MS_MAX_DISPLAYS];
    UInt32 startChId[CHAINS_SW_MS_MAX_DISPLAYS];

    Bool autoSwitchLayout;
    Bool autoSwitchCh;

    UInt32 timerCount;

} Chains_SwMsCtrl;

Chains_SwMsCtrl gChains_swMsCtrl;

UInt32 gChains_swMsLayoutList[] = {
    SYSTEM_LAYOUT_MODE_8CH,
    SYSTEM_LAYOUT_MODE_16CH,
    SYSTEM_LAYOUT_MODE_5CH_PLUS_1CH,
    SYSTEM_LAYOUT_MODE_7CH_PLUS_1CH,
    SYSTEM_LAYOUT_MODE_4CH,
    SYSTEM_LAYOUT_MODE_1CH,
    SYSTEM_LAYOUT_MODE_1CH_PLUS_2CH_PIP,
};

UInt8 gChains_swMsChList[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
};

/* ===================================================================
 *  @func     Chains_swMsTimerCb                                               
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
Void Chains_swMsTimerCb(UArg arg)
{
    gChains_swMsCtrl.timerCount++;

    if (gChains_swMsCtrl.timerCount %
        CHAINS_SW_MS_AUTO_LAYOUT_SWITCH_INTERVAL_IN_SECS == 0)
    {
        gChains_swMsCtrl.autoSwitchLayout = TRUE;
    }
    if (gChains_swMsCtrl.timerCount %
        CHAINS_SW_MS_AUTO_CH_SWITCH_INTERVAL_IN_SECS == 0)
    {
        gChains_swMsCtrl.autoSwitchCh = TRUE;
    }
}

/* ===================================================================
 *  @func     Chains_isSwMsAutoSwitchLayout                                               
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
Bool Chains_isSwMsAutoSwitchLayout(Bool autoSwitchEnable)
{
    if (!autoSwitchEnable)
        return FALSE;

    if (gChains_swMsCtrl.autoSwitchLayout)
    {
        gChains_swMsCtrl.autoSwitchLayout = FALSE;

        return TRUE;
    }

    return FALSE;
}

/* ===================================================================
 *  @func     Chains_isSwMsAutoSwitchCh                                               
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
Bool Chains_isSwMsAutoSwitchCh(Bool autoSwitchEnable)
{
    if (!autoSwitchEnable)
        return FALSE;

    if (gChains_swMsCtrl.autoSwitchCh)
    {
        gChains_swMsCtrl.autoSwitchCh = FALSE;

        return TRUE;
    }

    return FALSE;
}

/* ===================================================================
 *  @func     Chains_swMsGetLayoutPrm                                               
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
Void Chains_swMsGetLayoutPrm(UInt32 displayNum, Bool switchLayout,
                             Bool switchCh, SwMsLink_LayoutParams * pLayoutPrm)
{
    UInt32 numLayouts;

    UInt32 numCh;

    UInt32 winId, chId;

    GT_assert(GT_DEFAULT_MASK, displayNum < CHAINS_SW_MS_MAX_DISPLAYS);

    numLayouts =
        sizeof(gChains_swMsLayoutList) / sizeof(gChains_swMsLayoutList[0]);
    numCh = sizeof(gChains_swMsChList) / sizeof(gChains_swMsChList[0]);

    if (switchLayout)
    {
        gChains_swMsCtrl.layoutId[displayNum]++;
        if (gChains_swMsCtrl.layoutId[displayNum] >= numLayouts)
            gChains_swMsCtrl.layoutId[displayNum] = 0;
    }

    if (switchCh)
    {
        gChains_swMsCtrl.startChId[displayNum]++;
        if (gChains_swMsCtrl.startChId[displayNum] >= numCh)
            gChains_swMsCtrl.startChId[displayNum] = 0;

    }

    pLayoutPrm->outLayoutMode =
        gChains_swMsLayoutList[gChains_swMsCtrl.layoutId[displayNum]];

    chId = gChains_swMsCtrl.startChId[displayNum];

    for (winId = 0; winId < SYSTEM_SW_MS_MAX_WIN; winId++)
    {
        pLayoutPrm->win2ChMap[winId] = gChains_swMsChList[chId];

        chId = (chId + 1) % numCh;
    }
}

/* ===================================================================
 *  @func     Chains_swMsSwitchLayout                                               
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
Int32 Chains_swMsSwitchLayout(UInt32 swMsLinkId[CHAINS_SW_MS_MAX_DISPLAYS],
                              SwMsLink_CreateParams
                              swMsPrm[CHAINS_SW_MS_MAX_DISPLAYS],
                              Bool switchLayout, Bool switchCh,
                              UInt32 numDisplay)
{
    UInt32 i;

    if (!switchLayout && !switchCh)
        return FVID2_SOK;

    for (i = 0; i < numDisplay; i++)
    {
        Chains_swMsGetLayoutPrm(i, switchLayout, switchCh,
                                &swMsPrm[i].layoutPrm);
        System_linkControl(swMsLinkId[i], SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT,
                           &swMsPrm[i].layoutPrm, TRUE);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     Chains_swMsInit                                               
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
Int32 Chains_swMsInit()
{
    UInt32 displayNum;

    Clock_Params clockParams;

    memset(&gChains_swMsCtrl, 0, sizeof(gChains_swMsCtrl));

    gChains_swMsCtrl.autoSwitchLayout = FALSE;
    gChains_swMsCtrl.autoSwitchCh = FALSE;

    for (displayNum = 0; displayNum < CHAINS_SW_MS_MAX_DISPLAYS; displayNum++)
    {
        gChains_swMsCtrl.layoutId[displayNum] = 0;
        gChains_swMsCtrl.startChId[displayNum] = 0;

        if (displayNum == 2)
            gChains_swMsCtrl.layoutId[displayNum] = 1;
    }

    Clock_Params_init(&clockParams);
    clockParams.period = 1000;
    clockParams.arg = (UArg) & gChains_swMsCtrl;

    gChains_swMsCtrl.timer = Clock_create(Chains_swMsTimerCb,
                                          clockParams.period,
                                          &clockParams, NULL);
    GT_assert(GT_DEFAULT_MASK, gChains_swMsCtrl.timer != NULL);

    Clock_start(gChains_swMsCtrl.timer);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     Chains_swMsExit                                               
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
Int32 Chains_swMsExit()
{
    Clock_stop(gChains_swMsCtrl.timer);
    Clock_delete(&gChains_swMsCtrl.timer);

    return FVID2_SOK;
}
