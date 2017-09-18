/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SYSTEM_SW_MS_H_
#define _SYSTEM_SW_MS_H_

#include <ti/psp/examples/common/vps/chains/links/system.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>
#include <ti/psp/vps/vps_m2mDei.h>


#define SYSTEM_SW_MS_MAX_WIN           (16)
#define SYSTEM_SW_MS_MAX_CH_ID         (32)
#define SYSTEM_SW_MS_INVALID_ID        (0xFF)

/**
    \brief Link CMD: Switch channel that is being scaled by scaler

    \param SwMsLink_LayoutParams * [IN] Command parameters
*/
#define SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT        (0x5000)

typedef struct {

    UInt32 outRes;
    /**< output resolution SYSTEM_RES_xxx */

    UInt32 outLayoutMode;
    /**< Output layout ID: SYSTEM_LAYOUT_MODE_xxx */

    UInt8  win2ChMap[SYSTEM_SW_MS_MAX_WIN];

} SwMsLink_LayoutParams;

typedef struct
{
    UInt8  channelNum;

    UInt32 bufAddrOffset;

    UInt32 startX;
    UInt32 startY;
    UInt32 width;
    UInt32 height;

    Bool   bypass;

} SwMsLink_LayoutWinInfo;


typedef struct
{
    UInt32 layoutId;
    UInt32 numWin;

    SwMsLink_LayoutWinInfo winInfo[SYSTEM_SW_MS_MAX_WIN];

    UInt8  ch2WinMap[SYSTEM_SW_MS_MAX_CH_ID];

} SwMsLink_LayoutInfo;

/**
    \brief Link create parameters
*/
typedef struct
{
    System_LinkInQueParams  inQueParams;
    /**< input queue information */

    System_LinkOutQueParams outQueParams;
    /**< output queue information */

    SwMsLink_LayoutParams layoutPrm;
    /**< Layout parameters */

    UInt32                  timerPeriod;
    /**< Rate at which output frames should be generated,
         should typically be equal to the display rate

          Example, for 60fps display, timerPeriod should be 1000/60 = 16
    */


} SwMsLink_CreateParams;

Int32 SwMsLink_getLayoutInfo(SwMsLink_LayoutParams *layoutParams, SwMsLink_LayoutInfo *layoutInfo, UInt32 outPitch);
Int32 SwMsLink_getOutSize(UInt32 outRes, UInt16 *width, UInt16 *height);

/**
    \brief Dei link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 SwMsLink_init();

/**
    \brief Dei link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 SwMsLink_deInit();


#endif

