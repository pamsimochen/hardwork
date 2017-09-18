/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _GRPX_LINK_PRIV_H_
#define _GRPX_LINK_PRIV_H_

#include <ti/psp/examples/common/vps/chains/links/grpxLink.h>
#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>

#define GRPX_LINK_BLANK_FRAME_CHANNEL_NUM     (0xFFFF)

#define GRPX_LINK_BLANK_FRAME_INIT_QUE        (6)

#define GRPX_LINK_OBJ_MAX (3)

#define GRPX_LINK_CMD_DO_DEQUE                (0x0500)

typedef struct
{
    UInt32 tskId;

    VpsUtils_TskHndl tsk;

    GrpxLink_CreateParams createArgs;

    System_LinkInfo      *pInTskInfo;

    System_LinkQueInfo   inQueInfo;

    Semaphore_Handle    lock;
    Semaphore_Handle    complete;

    UInt32 curGrpxChannelNum;
    UInt32 queueCount;

    FVID2_Handle         grpxHndl;

    Vps_GrpxCreateParams grpxCreateArgs;
    Vps_GrpxCreateStatus grpxCreateStatus;
    FVID2_Format         grpxFormat;
    UInt32               grpxInstId;
    /*graphics region list*/
    Vps_GrpxParamsList    grpxParamsList;
    
    /* Maximum 16 regions possible */
    Vps_GrpxRtParams      grpxRegionParams[16];

    FVID2_Frame          blankFrame;

    UInt32               dequeCount;
    UInt32               cbCount;

    UInt32               totalTime;

    UInt32               startTime;
    UInt32               prevTime;
    UInt32               minCbTime;
    UInt32               maxCbTime;
    Uint32               lastCbTime;

} GrpxLink_Obj;



Int32 GrpxLink_drvCreate(GrpxLink_Obj *pObj, GrpxLink_CreateParams *pPrm);
Int32 GrpxLink_drvStart(GrpxLink_Obj *pObj);
Int32 GrpxLink_drvProcessData(GrpxLink_Obj *pObj);
Int32 GrpxLink_drvStop(GrpxLink_Obj *pObj);
Int32 GrpxLink_drvDelete(GrpxLink_Obj *pObj);
Int32 GrpxLink_drvLock(GrpxLink_Obj *pObj);
Int32 GrpxLink_drvUnlock(GrpxLink_Obj *pObj);
Int32 GrpxLink_drvSwitchCh(GrpxLink_Obj *pObj, UInt32 channelNum);

#endif
