/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _CHAINS_H_
#define _CHAINS_H_


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/psp/examples/common/vps/chains/links/system.h>
#include <ti/psp/examples/common/vps/chains/links/captureLink.h>
#include <ti/psp/examples/common/vps/chains/links/scalarSwMsLink.h>
#include <ti/psp/examples/common/vps/chains/links/deiLink.h>
#include <ti/psp/examples/common/vps/chains/links/nsfLink.h>
#include <ti/psp/examples/common/vps/chains/links/displayLink.h>
#include <ti/psp/examples/common/vps/chains/links/displayHwMsLink.h>
#include <ti/psp/examples/common/vps/chains/links/nullLink.h>
#include <ti/psp/examples/common/vps/chains/links/grpx.h>
#include <ti/psp/examples/common/vps/chains/links/scLink.h>
#include <ti/psp/examples/common/vps/chains/links/dupLink.h>
#include <ti/psp/examples/common/vps/chains/links/swMsLink.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils.h>

#define CHAINS_DISPLAY_ID_HDMI_ONCHIP   (0)
#define CHAINS_DISPLAY_ID_HDMI_OFFCHIP  (1)
#define CHAINS_DISPLAY_ID_NONE  (2)

#define CHAINS_DEI_ID_HQ    (0)
#define CHAINS_DEI_ID       (1)

typedef struct {

    /* execution time of the chain in seconds */
    UInt32 exeTimeInSecs;

    /* number of time to loop the execution, including create/start/stop/delete */
    UInt32 loopCount;

    /* number of VIP instances to use for capture, controls
        number of channels that are processed */
    UInt32 numVipInst;
    UInt32 VipPort;
    UInt32 numSubChains;

    UInt32 numDisplays;

    /* display[0] - display to use, when in single display mode

       display[0], display[1] - display to use, when in dual display mode
    */
    UInt32 displayId[2];

    /* DEI to use when in single DEI chain mode */
    UInt32 deiId;

    /* DEI output to display when DEI in dual output and single display mode

        DEI_LINK_OUT_QUE_DEI_SC or DEI_LINK_OUT_QUE_VIP_SC
    */
    UInt32 deiOutDisplayId;

    /* Enable NSF during chain */
    Bool   enableNsfLink;

    /* NSF mode when NSF Link is in chain

        TRUE: NSF is bypass and is in CHR DS mode
        FALSE: NSF is bypass and is in CHR DS mode
    */
    Bool bypassNsf;

    Bool grpxEnable;

    Vps_PlatformCpuRev  cpuRev;
    Vps_PlatformBoardId boardId;

    UInt32 displayRes;
    UInt32 sdDisplayRes;

    Bool tilerEnable;
    Bool comprEnable;

    Bool captureSingleChOutYuv420;
    Bool enableDisSyncMode;

    Bool captureSingleChOutYuv422SP;

    /* Used only by catalog chains as of now. Used to set TVP7002 for
       progressive mode. */
    Bool tvp7002InProgressiveMode;
    Bool autoRunEnable;

    /* Enables switching of output sizes for scaler. */
    Bool captureScSwitchEnable;
    /* Enable/Disable EDE */
    Bool edeEnable;
} Chains_Ctrl;

typedef Void (*Chains_RunFunc)();

Void Chains_singleChCaptureSii9135(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureNsfDeiTvp7002(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureTvp7002DisSync(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureSii9135Tvp7002(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureTvp7002(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureTvp7002FieldMerged(Chains_Ctrl *chainsCfg);

Void Chains_multiChCaptureNsf(Chains_Ctrl *chainsCfg);
Void Chains_multiChCaptureHwMsDisplay(Chains_Ctrl *chainsCfg);
Void Chains_multiChCaptureNsfDei(Chains_Ctrl *chainsCfg);
Void Chains_multiChSystemUseCaseHwMsSingleDisplay(Chains_Ctrl *chainsCfg);
Void Chains_multiChSystemUseCaseHwMsTriDisplay(Chains_Ctrl *chainsCfg);
Void Chains_multiChSystemUseCaseSwMsTriDisplay(Chains_Ctrl *chainsCfg);
Void Chains_multiChSystemUseCaseSwMsTriDisplay2(Chains_Ctrl *chainsCfg);
Void Chains_tvp5158NonMuxCapture(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureNsfDeiTvp5158(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureNsfDeiSii9135_480i(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureNsfDeiSii9135_1080i(Chains_Ctrl *chainsCfg);
Void Chains_singleChCaptureNsfDeiSii9135_480i_fullscreen(Chains_Ctrl *chainsCfg);

Void Chains_menuSettings();
Void Chains_menuCurrentSettingsShow();

Int32 Chains_detectBoard();

#endif

