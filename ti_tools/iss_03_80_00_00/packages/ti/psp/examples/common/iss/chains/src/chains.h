/** ==================================================================
 *  @file   chains.h                                                  
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

#ifndef _CHAINS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CHAINS_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/psp/examples/common/iss/chains/links/system.h>
#include <ti/psp/examples/common/iss/chains/links/cameraLink.h>
#include <ti/psp/examples/common/iss/chains/links/scalarSwMsLink.h>
#include <ti/psp/examples/common/iss/chains/links/deiLink.h>
#include <ti/psp/examples/common/iss/chains/links/nsfLink.h>
#include <ti/psp/examples/common/iss/chains/links/displayLink.h>
#include <ti/psp/examples/common/iss/chains/links/displayHwMsLink.h>
#include <ti/psp/examples/common/iss/chains/links/nullLink.h>
#include <ti/psp/examples/common/iss/chains/links/grpx.h>
#include <ti/psp/examples/common/iss/chains/links/scLink.h>
#include <ti/psp/examples/common/iss/chains/links/dupLink.h>
#include <ti/psp/examples/common/iss/chains/links/swMsLink.h>
#include <ti/psp/platforms/iss_platform.h>
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

    /* number of time to loop the execution, including
     * create/start/stop/delete */
    UInt32 loopCount;

    /* number of VIP instances to use for capture, controls number of
     * channels that are processed */
    UInt32 numVipInst;
    UInt32 VipPort;
    UInt32 numSubChains;

    UInt32 numDisplays;

    /* display[0] - display to use, when in single display mode
     * 
     * display[0], display[1] - display to use, when in dual display mode */
    UInt32 displayId[2];

    /* DEI to use when in single DEI chain mode */
    UInt32 deiId;

    /* DEI output to display when DEI in dual output and single display mode
     * 
     * DEI_LINK_OUT_QUE_DEI_SC or DEI_LINK_OUT_QUE_VIP_SC */
    UInt32 deiOutDisplayId;

    /* Enable NSF during chain */
    Bool enableNsfLink;

    /* NSF mode when NSF Link is in chain
     * 
     * TRUE: NSF is bypass and is in CHR DS mode FALSE: NSF is bypass and is
     * in CHR DS mode */
    Bool bypassNsf;

    Bool grpxEnable;

    Iss_PlatformCpuRev cpuRev;
    Iss_PlatformBoardId boardId;

    UInt32 displayRes;
    UInt32 sdDisplayRes;

    Bool tilerEnable;
    Bool comprEnable;

    Bool cameraSingleChOutYuv420;
    Bool enableDisSyncMode;

    Bool cameraSingleChOutYuv422SP;

    /* Used only by catalog chains as of now. Used to set TVP7002 for
     * progressive mode. */
    Bool tvp7002InProgressiveMode;

} Chains_Ctrl;

typedef Void(*Chains_RunFunc) ();

Void Chains_CameraMt9j003(Chains_Ctrl * chainsCfg);

/* ===================================================================
 *  @func     Chains_menuSettings                                               
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
Void Chains_menuSettings();

/* ===================================================================
 *  @func     Chains_menuCurrentSettingsShow                                               
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
Void Chains_menuCurrentSettingsShow();

/* ===================================================================
 *  @func     Chains_detectBoard                                               
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
Int32 Chains_detectBoard();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
